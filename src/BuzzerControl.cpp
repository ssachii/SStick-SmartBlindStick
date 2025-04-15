#include "BuzzerControl.h"

BuzzerControl::BuzzerControl(int buzzerPin) {
  pin = buzzerPin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // Active-low buzzer off
}

void BuzzerControl::buzz(int frequency, int duration) {
  tone(pin, frequency);
  buzzing = true;
  buzzEndTime = millis() + duration;
}

void BuzzerControl::fromUltrasound() {
  buzz(1000, 100);
}

void BuzzerControl::fromWater() {
  buzz(750, 200);
}

void BuzzerControl::fromGyro() {
  Serial.println("Calling buzzer.fromGyro()");
  pinMode(pin, OUTPUT);
  buzz(2000, 10000);  // Loud for 10 seconds

}

void BuzzerControl::fromPIR() {
  pirBeepActive = true;
  pirBeepStep = 0;
  pirNextBeepTime = millis();
}

void BuzzerControl::update() {
  unsigned long now = millis();

  if (buzzing && now > buzzEndTime) {
    off();
  }

  if (pirBeepActive && now >= pirNextBeepTime) {
    if (pirBeepStep == 0 || pirBeepStep == 2) {
      tone(pin, 300);
      buzzEndTime = now + 100;
      buzzing = true;
      pirBeepStep++;
      pirNextBeepTime = now + 100;
    } else if (pirBeepStep == 1 || pirBeepStep == 3) {
      off();
      pirBeepStep++;
      pirNextBeepTime = now + 100;
    }

    if (pirBeepStep >= 4) {
      pirBeepActive = false;
    }
  }
}

void BuzzerControl::off() {
  noTone(pin);
  pinMode(pin, INPUT);
  buzzing = false;
}
