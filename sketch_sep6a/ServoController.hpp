#pragma once

#include <Arduino.h>
#include <Servo.h>

struct ServoController {
  ServoController(uint8_t servo_pin);

  void handleTick();
  void controlServo(bool is_user_passed_2fa);

private:
  bool isServoInProgress();

  long servo_timeout = 5000;
  long servo_turned_on_last_timestamp{};
  bool servo_is_open{ false };

  uint8_t servo_pin;
  Servo servoMotor;
};
