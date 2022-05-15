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

Servo servo;
Led rLED;
Led yLED;
const byte buzzPin = 3;
const byte btnPin = 2;
const byte pirSensorPin = 4;
int peepFrequency = 800;
int servoLockedPosition = 90;
int lockingTime = 5000;
int warningTime = 3000;
const byte keypadRows = 4;
const byte keypadColumns = 3;
const byte rowsPins[keypadRows] = {12, 11, 10, 9};
const byte columnsPins[keypadColumns] = {8, 7, 6};
String password = "123";

char keypadCharacters[keypadRows][keypadColumns] = {
   {'1','2','3'},
   {'4','5','6'},
   {'7','8','9'},
   {'*','0','#'},
};
Keypad keyPad = Keypad(makeKeymap(keypadCharacters), rowsPins, columnsPins, keypadRows, keypadColumns);

void setup() {
  servo.attach(5);
  yLED.setup(0);
  rLED.setup(1);
  pinMode(buzzPin, OUTPUT);
  pinMode(btnPin, INPUT);
  pinMode(pirSensorPin, INPUT);
  Serial.begin(9600);
  
  password += '#';
}

void loop() {
  bool btnPressed;
  bool moveDetected;
  bool rightPasswordEntered;
  unlocked();
  btnPressed = digitalRead(btnPin);
  if (btnPressed) {
    int countingTime = 0;
    unsigned long int startCountingTime = millis();
    while (countingTime <= lockingTime) {
      locking();
      countingTime = millis() - startCountingTime; 
    }
    moveDetected = digitalRead(pirSensorPin);
    while (!moveDetected && !rightPasswordEntered) {
     locked();
     moveDetected = digitalRead(pirSensorPin);
     String inputPassword = waitForPassword();
     rightPasswordEntered = inputPassword == password;
    }
    if (moveDetected) {
      int countingTime = 0;
      unsigned long int startCountingTime = millis();
      btnPressed = digitalRead(btnPin);
      while (countingTime <= warningTime && !btnPressed) {
       warning();
       countingTime = millis() - startCountingTime;
       btnPressed = digitalRead(btnPin);
      }
      if (countingTime > warningTime) {
        while (!btnPressed) {
          alarm();
          btnPressed = digitalRead(btnPin);
        }
      }
    }
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

String waitForPassword() {
  String pwd = "";
  char key = keyPad.getKey();
  if (key == '*') {
    Serial.println("* pressed");
    while (key != '#') {
      key = keyPad.getKey();
      pwd += key;
      delay(10);
    }
    return pwd;
  }
}
