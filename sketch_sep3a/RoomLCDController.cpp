#include "RoomLCDController.hpp"

RoomLCDController::RoomLCDController() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Start");
}

void RoomLCDController::handleTick() {
  if (was_occupied_by_external_event && !isOccupiedByExternalEvent()) {
    updateDisplay(overriden_first_line, overriden_second_line);
  }

  was_occupied_by_external_event = isOccupiedByExternalEvent();
}

void RoomLCDController::updateDisplay(const std::string& new_first_line, const std::string& new_second_line) {
  first_line = new_first_line;
  second_line = new_second_line;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(first_line.c_str());
  lcd.setCursor(0, 1);
  lcd.print(second_line.c_str());

  was_occupied_by_external_event = false;
}

void RoomLCDController::updateDisplayExternalEvent(const std::string& new_first_line, const std::string& new_second_line) {
  if (!was_occupied_by_external_event) {
    overriden_first_line = first_line;
    overriden_second_line = second_line;
  }

  external_even_timestamp = millis();
  was_occupied_by_external_event = true;

  updateDisplay(new_first_line, new_second_line);
}

bool RoomLCDController::isOccupiedByExternalEvent() const {
  return millis() <= external_even_timestamp + EXTERNAL_EVENT_OCCUPATION_TIME_LIMIT;
}
