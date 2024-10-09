#pragma once

#include <Arduino.h>

class WaterPumpController {
public:
  WaterPumpController(uint8_t motor_relay_pin);

  void controlMotor(bool is_system_on, int water_level, bool is_room_force_off_water_supply);

  static constexpr int WATER_LEVEL_THRESHOLD = 250;

private:
  uint8_t motor_relay_pin;
};
