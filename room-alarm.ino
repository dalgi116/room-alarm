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
Led arLED;
Led ayLED;
Led krLED;
Led kgLED;
const byte buzzPin = 6;
const byte btnPin = 7;
const byte pirSensorPin = 9;
const byte keypadRows = 4;
const byte keypadColumns = 3;
const byte rowsPins[keypadRows] = {A0, A1, A2, A3};
const byte columnsPins[keypadColumns] = {A4, A5, 10};

char keypadCharacters[keypadRows][keypadColumns] = {
   {'1','2','3'},
   {'4','5','6'},
   {'7','8','9'},
   {'*','0','#'},
};
Keypad keyPad = Keypad(makeKeymap(keypadCharacters), rowsPins, columnsPins, keypadRows, keypadColumns);

void setup() {
  servo.attach(8);
  ayLED.setup(3);
  arLED.setup(2);
  krLED.setup(4);
  kgLED.setup(5);
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
    if (passwordsMatches) {
      kgLED.on();
      krLED.off();
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
        if (passwordsMatches) {
          kgLED.on();
          krLED.off();
        }
      }
    }
    tone(buzzPin, peepFrequency);
    delay(1000);
  }
}


// alarm state functions
void unlocked() {
  arLED.off();
  ayLED.off();
  krLED.off();
  kgLED.off();
  noTone(buzzPin);
  servo.write(0);
}

void locking() {
  int interval = 1500;
  arLED.off();
  ayLED.flash(interval);
  peep(interval);
  servo.write(0);
}

void locked() {
  arLED.off();
  ayLED.on();
  krLED.on();
  noTone(buzzPin);
  servo.write(servoLockedPosition);
}

void warning() {
  int interval = 1000;
  arLED.flash(interval);
  ayLED.on();
  peep(interval);
  servo.write(servoLockedPosition);
}

void alarm() {
  int interval = 500;
  arLED.flash(interval);
  ayLED.on();
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
