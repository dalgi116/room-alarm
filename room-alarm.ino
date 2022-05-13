class Led {
  public:
    byte port;
    unsigned long int blinkCounter = 0;

    void setup(byte iPin) {
      port = iPin;
      pinMode(port, OUTPUT);
    }
    void blink(int blinkInterval) {
      static bool state = LOW;
      if (blinkCounter <= millis()) {
        if (state == HIGH) {
          state = LOW;
        } else {
          state = HIGH;
        }
        digitalWrite(port, state);
        blinkCounter = millis() + blinkInterval;
      }
    }
};

Led rLED;
Led yLED;

void setup() {
  yLED.setup(0);
  rLED.setup(1);
}

void loop() {
  rLED.blink(500);
}



void unlocked() {
}

void locking() {
}

void locked() {
}

void warning() {
}

void alarm() {
}
