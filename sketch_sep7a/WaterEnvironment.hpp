#pragma once

#include <Arduino.h>

class WaterEnvironment {
public:
  WaterEnvironment(uint8_t motor_system_switch, uint8_t water_sensor_pin);

  bool isSystemOn();
  int getWaterLevel();

private:
  uint8_t motor_system_switch;
  uint8_t water_sensor_pin;
};