#include "RoomMotionSensor.hpp"

RoomMotionSensor::RoomMotionSensor(
  uint8_t motion_sensor_pin)
  : motion_sensor_pin(motion_sensor_pin) {
  pinMode(motion_sensor_pin, INPUT);
}

void RoomMotionSensor::handleTick() {
  if (isMotionDetected()) {
    last_motion_time = millis();
    //  Serial.println("OK");
  } else {
    //  Serial.println("----");
  }
}

bool RoomMotionSensor::hasMotionExpired() const {
  return (millis() - last_motion_time) >= MOTION_EXPIRY_GRACE_PERIOD;
}

bool RoomMotionSensor::isMotionDetected() const {
  return digitalRead(motion_sensor_pin) == HIGH;
}
