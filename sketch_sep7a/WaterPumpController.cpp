#include "WaterPumpController.hpp"

WaterPumpController::WaterPumpController(uint8_t motor_relay_pin)
  : motor_relay_pin(motor_relay_pin) {
  pinMode(motor_relay_pin, OUTPUT);
  digitalWrite(motor_relay_pin, LOW);
}

void WaterPumpController::controlMotor(bool is_system_on, int water_level, bool is_room_force_off_water_supply) {
  if (!is_room_force_off_water_supply && is_system_on && water_level < WATER_LEVEL_THRESHOLD) {
    Serial.println("WHY");
    digitalWrite(motor_relay_pin, HIGH);
    return;
  }
  Serial.println("NO WHY");
  digitalWrite(motor_relay_pin, LOW);
}
