#pragma once

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <string>

struct RoomLCDController {

  RoomLCDController();
  void handleTick();
  void updateDisplay(const std::string& new_first_line, const std::string& new_second_line);
  void updateDisplayExternalEvent(const std::string& new_first_line, const std::string& new_second_line);

private:
  bool isOccupiedByExternalEvent() const;

  std::string first_line;
  std::string second_line;
  bool was_occupied_by_external_event{ false };
  std::string overriden_first_line;
  std::string overriden_second_line;
  unsigned long external_even_timestamp{};

  LiquidCrystal_I2C lcd{ 0x27, 16, 2 };

  static constexpr unsigned long EXTERNAL_EVENT_OCCUPATION_TIME_LIMIT = 5000;
};
