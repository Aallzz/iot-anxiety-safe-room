#pragma once

#include <Arduino.h>

struct RoomMotionSensor {
  RoomMotionSensor(uint8_t motion_sensor_pin);

  void handleTick();

  bool hasMotionExpired() const;

  static constexpr int MOTION_EXPIRY_GRACE_PERIOD = 10000;

private:
  bool isMotionDetected() const;

  unsigned long last_motion_time{};

  uint8_t motion_sensor_pin;
};
