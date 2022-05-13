#include <Servo.h>

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

void setup() {
  servo.attach(5);
  yLED.setup(0);
  rLED.setup(1);
  pinMode(buzzPin, OUTPUT);
  pinMode(btnPin, INPUT);
  pinMode(pirSensorPin, INPUT);
}

void loop() {
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
