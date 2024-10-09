#include "PasswordSensor.hpp"

PasswordSensor::PasswordSensor(std::vector<uint8_t> button_pins)
  : button_pins(button_pins) {
  for (uint8_t button_pin : button_pins) {
    pinMode(button_pin, INPUT);
    button_released.push_back(true);
  }
}

void PasswordSensor::startPasswordCollection() {
  collected_password.clear();
  is_password_collection_in_progress = true;
}

void PasswordSensor::stopPasswordCollection() {
  is_password_collection_in_progress = false;
}

void PasswordSensor::handleTick() {
  if (!is_password_collection_in_progress) {
    return;
  }

  for (size_t index = 0; index < button_pins.size(); ++index) {
    tryRecordButtonClickWithValue(index);
  }
}

std::vector<int> PasswordSensor::getPassword() const {
  return collected_password;
}

std::string PasswordSensor::getPasswordAsString() const {
  std::string password;
  for (int i : collected_password) {
    password += std::to_string(i);
  }
  return password;
}

void PasswordSensor::tryRecordButtonClickWithValue(size_t button_pin_index) {
  if (digitalRead(button_pins[button_pin_index]) == HIGH) {
    if (button_released[button_pin_index]) {
      collected_password.push_back(button_pin_index);
      button_released[button_pin_index] = false;
    }
  } else {
    button_released[button_pin_index] = true;
  }
}
