#ifndef VIBRATIONCONTROL_H
#define VIBRATIONCONTROL_H

#include <Arduino.h>

class VibrationControl {
  private:
    int pin;
    bool active = false;
    unsigned long endTime = 0;

  public:
    VibrationControl(int vibPin);
    void fromUltrasound();
    void fromPIR();
    void fromWater();
    void update();  // Non-blocking handler
    void off();
};

#endif
