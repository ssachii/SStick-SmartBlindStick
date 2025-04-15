#include "LEDControl.h"

LEDControl::LEDControl(int ledPin) {
  pin = ledPin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void LEDControl::fromPIR() {
  digitalWrite(pin, HIGH);
}

void LEDControl::off() {
  digitalWrite(pin, LOW);
}
