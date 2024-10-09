#include "WaterEnvironmentMessageBuilder.hpp"
#include <Arduino.h>

std::string WaterEnvironmentMessageBuilder::buildJSONWaterLevelEventMessage(
  bool is_system_on, 
  int water_level, 
  bool is_room_force_off_water_supply) {
  StaticJsonDocument<200> event_data;
  event_data["controller_id"] = "water-controller";
  event_data["water_level"] = water_level;
  event_data["is_system_on"] = is_system_on;
  event_data["is_room_force_off_water_supply"] = is_room_force_off_water_supply;

  StaticJsonDocument<200> doc;
  doc["event"] = "water_level";
  doc["event_data"] = event_data;

  String jsonMessage;
  serializeJson(doc, jsonMessage);

  return std::string(jsonMessage.c_str());
}