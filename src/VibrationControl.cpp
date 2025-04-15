#include "VibrationControl.h"

VibrationControl::VibrationControl(int vibPin) {
  pin = vibPin;
  // Don't touch pinMode/digitalWrite here
}

void VibrationControl::fromUltrasound() {
  digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
  active = true;
  endTime = millis() + 100;
}

void VibrationControl::fromPIR() {
  digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
  active = true;
  endTime = millis() + 50;
}

void VibrationControl::fromWater() {
  digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
  active = true;
  endTime = millis() + 50;
}

void VibrationControl::update() {
  if (active && millis() > endTime) {
    off();
  }
}

void VibrationControl::off() {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
  active = false;
}
