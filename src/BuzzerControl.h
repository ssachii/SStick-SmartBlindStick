#ifndef BUZZER_CONTROL_H
#define BUZZER_CONTROL_H

#include <Arduino.h>

class BuzzerControl {
  private:
    int pin;
    bool buzzing = false;
    unsigned long buzzEndTime = 0;

    // For PIR double-beep logic
    bool pirBeepActive = false;
    int pirBeepStep = 0;
    unsigned long pirNextBeepTime = 0;

  public:
    BuzzerControl(int buzzerPin);
    void buzz(int frequency, int duration);
    void fromUltrasound();
    void fromPIR();
    void fromWater();
    void fromGyro();
    void update(); // Call this regularly in loop()
    void off();
};

#endif
