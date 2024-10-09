// The entrypoint to Node responsible for the water supply control
// It is using data from the main server passed by websocket water_websocket 
// , data from water sensor (WATER_SENSOR_PIN) and switch for disabling/enabling
// the whole node (MOTOR_SYSTEM_SWITCH) to control the water pump motor using
// transistor and relay (for increase of voltage from 3.3 to 5 V)

#include <memory>

#include <ArduinoJson.h>

// Simple util class that connects to the WiFi 
#include "WifiConnection.hpp"

// Util class that allows to attach handlers to the web socket events
// and broadcast messsages 
#include "WaterWebSocket.hpp"

// Collect sensory data from the pins related to the node returning 
// context important values like is system on and how the water sensor feels
#include "WaterEnvironment.hpp"

// Class that tells what motor should do based on the arguments passed 
#include "WaterPumpController.hpp"

// A builder that is sort of data wrapper for creating messages
// that this project three-Node system understands
#include "WaterEnvironmentMessageBuilder.hpp"

constexpr uint8_t WATER_SENSOR_PIN = A0;
constexpr uint8_t MOTOR_RELAY_PIN = D2;
constexpr uint8_t MOTOR_SYSTEM_SWITCH = D5;

WifiConnection wifi_connection;
std::unique_ptr<WaterWebSocket> water_websocket;
std::unique_ptr<WaterEnvironment> water_environment;
std::unique_ptr<WaterPumpController> water_pump_controller;

bool is_room_force_off_water_supply {true};

void setup() {
  Serial.begin(4800);
  delay(1000);

  wifi_connection.connect();

  water_websocket = std::make_unique<WaterWebSocket>();
  water_websocket->registerWebSocketHandler("room_update", [&is_room_force_off_water_supply](const std::string& data) {
    String message = data.c_str();
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.println("Failed to parse JSON from WebSocket message");
      return;
    }

    int people_in_room_cnt = static_cast<int>(doc["people_in_room_cnt"]);
    bool no_movement_in_room_cnt = static_cast<bool>(doc["no_movement_in_room"]);
    is_room_force_off_water_supply = people_in_room_cnt == 0 || no_movement_in_room_cnt;
  });
  water_websocket->connect();

  water_environment = std::make_unique<WaterEnvironment>(
    MOTOR_SYSTEM_SWITCH,
    WATER_SENSOR_PIN);

  water_pump_controller = std::make_unique<WaterPumpController>(
    MOTOR_RELAY_PIN);
}

void loop() {
  delay(300);

  water_websocket->handleTick();

  water_pump_controller->controlMotor(
    water_environment->isSystemOn(), 
    water_environment->getWaterLevel(),
    is_room_force_off_water_supply
  );

  water_websocket->broadcastViaWebsocket(
    WaterEnvironmentMessageBuilder::buildJSONWaterLevelEventMessage(
      water_environment->isSystemOn(),
      water_environment->getWaterLevel(),
      is_room_force_off_water_supply));
}
