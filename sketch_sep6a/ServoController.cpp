#include "ServoController.hpp"

ServoController::ServoController(uint8_t servo_pin)
  : servo_pin(servo_pin) {
  servoMotor.attach(servo_pin);
  servoMotor.write(0);
}

void ServoController::handleTick() {
  if (servo_is_open && servo_turned_on_last_timestamp + servo_timeout <= millis()) {
    servo_is_open = false;
    servoMotor.write(0);
  }
}

void ServoController::controlServo(bool is_user_passed_2fa) {
  if (is_user_passed_2fa && !servo_is_open) {
    servo_is_open = true;
    servo_turned_on_last_timestamp = millis();
    servoMotor.write(180);
  }
}

bool ServoController::isServoInProgress() {
  return servo_turned_on_last_timestamp + servo_timeout >= millis();
}
