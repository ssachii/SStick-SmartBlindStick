#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include "BuzzerControl.h"
#include "VibrationControl.h"
#include "LEDControl.h"

const int trigPin = 8;
const int echoPin = 9;
const int pirPin = 2;
const int waterSensorPin = A0;
const int touchPin1 = 7;
const int touchPin2 = 10;

BuzzerControl buzzer(4);
VibrationControl vibration(5);
LEDControl led(6);

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

unsigned long lastPIRTrigger = 0;
const unsigned long pirCooldown = 2000;
bool motionPreviouslyDetected = false;

bool fallDetected = false;
bool waitingForTouchReset = false;
float lastAcc = 9.81;  // To compute acceleration delta

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(waterSensorPin, INPUT);
  pinMode(touchPin1, INPUT);
  pinMode(touchPin2, INPUT);

  // === ✅ Touch required to start system ===
  Serial.println("🤚 Waiting for touch to start...");
  while (digitalRead(touchPin1) == LOW && digitalRead(touchPin2) == LOW) {
    delay(100);
  }
  Serial.println("✅ Touch detected. Booting system...");
  buzzer.buzz(1500, 150);
  delay(200);
  buzzer.off();

  if (!accel.begin()) {
    Serial.println("❌ ADXL345 not found. Check wiring.");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("✅ ADXL345 Ready");


  Serial.println("Warming up PIR Sensor ...")
  for (int i = 30; i > 0; i--) {
    Serial.print("⏳ ");
    Serial.print(i);
    Serial.println(" sec left");
    delay(1000);
  }

  Serial.println("System Ready ✅");
  buzzer.buzz(1000, 2000);
  delay(2000);
  buzzer.off();
  vibration.off();
  led.off();
}

void loop() {
  unsigned long now = millis();

  // === Touch Input Check ===
  bool touchActive = (digitalRead(touchPin1) == HIGH) || (digitalRead(touchPin2) == HIGH);

  // === Refined Fall Detection ===
  sensors_event_t event;
  accel.getEvent(&event);
  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;

  float rawAcc = sqrt(ax * ax + ay * ay + az * az);
  float netAcc = abs(rawAcc - 9.81);
  float delta = abs(rawAcc - lastAcc);
  lastAcc = rawAcc;

  if (!fallDetected && netAcc > 10.0 && delta > 8.0) {
    Serial.println("🚨 FALL DETECTED!");
    buzzer.fromGyro();
    fallDetected = true;
    waitingForTouchReset = false;
  }

  if (fallDetected) {
    buzzer.update();
    if (!touchActive && !waitingForTouchReset) {
      Serial.println("🕒 Touch released. Waiting for reactivation...");
      waitingForTouchReset = true;
    }
    if (touchActive && waitingForTouchReset) {
      Serial.println("✅ Touch reset complete. System resuming.");
      buzzer.off();
      vibration.off();
      fallDetected = false;
      waitingForTouchReset = false;
    }
    return;
  }

  // === PIR SENSOR ===
  int pirVal = digitalRead(pirPin);
  Serial.print("PIR State: ");
  Serial.println(pirVal);

  if (pirVal == HIGH) {
    led.fromPIR();
    if (!motionPreviouslyDetected && (now - lastPIRTrigger > pirCooldown)) {
      Serial.println("⚠️ Motion Detected by PIR!");
      buzzer.fromPIR();
      vibration.fromPIR();
      motionPreviouslyDetected = true;
      lastPIRTrigger = now;
    }
  } else {
    led.off();
    if (motionPreviouslyDetected) {
      Serial.println("✅ PIR reset to no motion");
      motionPreviouslyDetected = false;
    }
  }

  // === ULTRASONIC SENSOR ===
  long duration = getUltrasonicDuration();
  long cm = microsecondsToCentimeters(duration);

  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");

  if (cm > 0 && cm < 100) {
    buzzer.fromUltrasound();
    vibration.fromUltrasound();
  }

  // === WATER SENSOR ===
  int waterRaw = analogRead(waterSensorPin);
  int moisture = 100 - ((waterRaw / 1023.0) * 100);

  Serial.print("Water Sensor Value = ");
  Serial.print(waterRaw);
  Serial.print(" (");
  Serial.print(moisture);
  Serial.println("% moisture)");

  if (waterRaw < 600) {
    Serial.println("💧 Low moisture detected! Triggering water alert.");
    buzzer.fromWater();
    vibration.fromWater();
  }

  buzzer.update();
  vibration.update();

  delay(50);
}

long getUltrasonicDuration() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH);
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

