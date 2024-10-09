#pragma once

#include <ArduinoJson.h>
#include <string>

class WaterEnvironmentMessageBuilder {
public:
  static std::string buildJSONWaterLevelEventMessage(
    bool is_system_on,
    int water_level,
    bool is_room_force_off_water_supply);
};