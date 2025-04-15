#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Arduino.h>

class LEDControl {
  private:
    int pin;
  public:
    LEDControl(int ledPin);
    void fromPIR();
    void off();
};

#endif
