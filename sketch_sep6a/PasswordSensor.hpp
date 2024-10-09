#pragma once

#include <Arduino.h>
#include <vector>
#include <string>

struct PasswordSensor {
  PasswordSensor(std::vector<uint8_t> button_pins);

  void startPasswordCollection();
  void stopPasswordCollection();
  void handleTick();

  std::vector<int> getPassword() const;
  std::string getPasswordAsString() const;

private:
  void tryRecordButtonClickWithValue(size_t button_pin_index);

  std::vector<uint8_t> button_pins;
  std::vector<bool> button_released;

  bool is_password_collection_in_progress{ false };
  std::vector<int> collected_password{};
};
