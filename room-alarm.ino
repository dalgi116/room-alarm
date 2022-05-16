#include <Servo.h>
#include <Keypad.h>

class Led {
  public:
    byte port;

    void setup(byte iPin) {
      port = iPin;
      pinMode(port, OUTPUT);
    }
    void flash(int flashInterval) {
      static unsigned long int flashCounter = 0;
      static bool state = LOW;
      if (flashCounter <= millis()) {
        if (state == HIGH) {
          state = LOW;
        } else {
          state = HIGH;
        }
        digitalWrite(port, state);
        flashCounter = millis() + flashInterval;
      }
    }
    void on() {
      digitalWrite(port, HIGH);
    }
    void off() {
      digitalWrite(port, LOW);
    }
};

int peepFrequency = 800;
int servoLockedPosition = 90;
int lockingTime = 10000;
int warningTime = 5000;
int keypadResetTime = 10000;
String password = "*111";

Servo servo;
Led rLED;
Led yLED;
const byte buzzPin = 4;
const byte btnPin = A5;
const byte pirSensorPin = 5;
const byte keypadRows = 4;
const byte keypadColumns = 3;
const byte rowsPins[keypadRows] = {13, 12, 11, 10};
const byte columnsPins[keypadColumns] = {9, 8, 7};

char keypadCharacters[keypadRows][keypadColumns] = {
   {'1','2','3'},
   {'4','5','6'},
   {'7','8','9'},
   {'*','0','#'},
};
Keypad keyPad = Keypad(makeKeymap(keypadCharacters), rowsPins, columnsPins, keypadRows, keypadColumns);

void setup() {
  servo.attach(6);
  yLED.setup(3);
  rLED.setup(2);
  pinMode(buzzPin, OUTPUT);
  pinMode(btnPin, INPUT);
  pinMode(pirSensorPin, INPUT);
}

bool btnPressed;
bool moveDetected;
bool writingPwd;
bool passwordsMatches;
String pwd;
String inputPwd;
int countingTime;
unsigned long int startCountingTime;

void loop() {
  unlocked();
  btnPressed = digitalRead(btnPin);
  if (btnPressed) {
    countingTime = 0;
    startCountingTime = millis();
    while (countingTime <= lockingTime) {
      locking();
      countingTime = millis() - startCountingTime; 
    }
    moveDetected = digitalRead(pirSensorPin);
    passwordsMatches = false;
    while (!moveDetected && !passwordsMatches) {
      locked();
      moveDetected = digitalRead(pirSensorPin);
      inputPwd = getInputPwd();
      passwordsMatches = inputPwd == password;
    }
    if (moveDetected) {
      countingTime = 0;
      startCountingTime = millis();
      btnPressed = digitalRead(btnPin);
      while (countingTime <= warningTime && !btnPressed) {
        warning();
        countingTime = millis() - startCountingTime;
        btnPressed = digitalRead(btnPin);
      }
      if (countingTime > warningTime) {
        passwordsMatches = false;
        while (!btnPressed && !passwordsMatches) {
          alarm();
          btnPressed = digitalRead(btnPin);
          inputPwd = getInputPwd();
          passwordsMatches = inputPwd == password;
        }
      }
    }
    tone(buzzPin, peepFrequency);
    delay(1000);
  }
}


// alarm state functions
void unlocked() {
  rLED.off();
  yLED.off();
  noTone(buzzPin);
  servo.write(0);
}

void locking() {
  int interval = 1500;
  rLED.off();
  yLED.flash(interval);
  peep(interval);
  servo.write(0);
}

void locked() {
  rLED.off();
  yLED.on();
  noTone(buzzPin);
  servo.write(servoLockedPosition);
}

void warning() {
  int interval = 1000;
  rLED.flash(interval);
  yLED.on();
  peep(interval);
  servo.write(servoLockedPosition);
}

void alarm() {
  int interval = 500;
  rLED.flash(interval);
  yLED.on();
  peep(interval);
  servo.write(servoLockedPosition);
}

// other functions
void peep(int peepInterval) {
  static unsigned long int peepCounter = 0;
  static bool peeping;
  if (peepCounter <= millis()) {
    if (peeping) {
      peeping = false;
      noTone(buzzPin);
    } else {
      peeping = true;
      tone(buzzPin, peepFrequency);
    }
    peepCounter = millis() + peepInterval;
  }
}

String getInputPwd() {
  char pressedKey = keyPad.getKey();
  if (pressedKey == '*') {
    pwd = "";
    writingPwd = true;
    countingTime = 0;
    startCountingTime = millis();
  } else if (pressedKey == '#') {
    writingPwd = false;
    return pwd;
  }
  if (writingPwd && pressedKey) {
    pwd += pressedKey;
  }
  countingTime = millis() - startCountingTime;
  if (countingTime > keypadResetTime) {
    writingPwd = false;
  }
  return "";
}
