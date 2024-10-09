// The main class for the three node system that contains a lot of logic for both sensor and actuators and networking 
// 
// This class sets the separation between sensory information and actions using Environment classes for one and 
// Controller classes for another. Here and in another module you will see them too.
// The node collects information about relative distance of the object crossing the doror from the door
// (ULTRASONIC_SENSOR_*) while the door is being crossed (LASER_EMITTER_PIN, MOTION_SENSOR_PIN). It also
// collects information about movements in the room (MOTION_SENSOR_PIN). Together they tell if th light should be on 
// or off and tell it to RoomLCDController.
// There is also RoomLCDController that serves assistant role to display information about setup and security door progress

#include <array>
#include <memory>
#include <string_view>
#include <functional>
#include <map>

#include <ArduinoJson.h>

// It's a sensor class that combines a few sensors (laser, phototransistor
// ultrasonic) to undestand if and how the door is being crossed 
#include "RoomDoorwaySensor.hpp"

// Simple util class to connect to wifi
#include "RoomWifiConnection.hpp"

// Server class that allows to register how we want to proceed when the node
// retrieves some HTTP requests or web socket events, the handlers are registered
// in setup function for the websockets it allows broadcasting
#include "RoomServer.hpp"

// Clien class that allows connecting to some serever and send HTTP requests
#include "RoomClient.hpp"

// Class that reads the motion sensor values and controls the expiration in the context
// of the node
#include "RoomMotionSensor.hpp"

// Class that leverages room doorway sensor and tracks how many people are in the room 
// and how to handle if people are leaving
#include "RoomOccupancyTracker.hpp"

// Just HTML code to display what requests this node is receiving in web browser
#include "HTMLMainPage.hpp"

// Class collecting all needed data from sensors and giving necessary data
// to the controllers
#include "RoomEnvironment.hpp"

// Super cool class to print text on LCD
#include "RoomLCDController.hpp"

// Yeelight lamp controller (my smart lamp)
#include "RoomLampController.hpp"

constexpr uint8_t ULTRASONIC_SENSOR_TRIGGER_PIN = D5;
constexpr uint8_t ULTRASONIC_SENSOR_ECHO_PIN = D6;
constexpr uint8_t LASER_EMITTER_PIN = D4;
constexpr uint8_t PHOTOTRANSISTOR_SENSOR_PIN = A0;
constexpr uint8_t MOTION_SENSOR_PIN = D7;

constexpr std::string_view SECURITY_SERVER_NAME = "security-controller.local";

RoomWifiConnection RoomWifiConnection;
std::unique_ptr<RoomLampController> lamp_controller;
std::shared_ptr<RoomLCDController> room_lcd_controller;
std::unique_ptr<RoomEnvironment> room_environment;
std::unique_ptr<RoomServer> room_server;
std::unique_ptr<RoomClient> room_client;

void setup() {
  Serial.begin(9600);

  RoomWifiConnection.connect();

  room_lcd_controller = std::make_shared<RoomLCDController>();

  room_client = std::make_unique<RoomClient>(std::string(SECURITY_SERVER_NAME));

  delay(500);
  room_lcd_controller->updateDisplay("Hello!", "I need a second.");

  delay(500);
  room_lcd_controller->updateDisplay("Room set up...", "");

  delay(500);
  room_lcd_controller->updateDisplay("Room sensors...", "");
  room_environment = std::make_unique<RoomEnvironment>(
    RoomMotionSensor{
      MOTION_SENSOR_PIN },
    (RoomOccupancyTracker{
       RoomDoorwaySensor{
         ULTRASONIC_SENSOR_TRIGGER_PIN,
         ULTRASONIC_SENSOR_ECHO_PIN,
         LASER_EMITTER_PIN,
         PHOTOTRANSISTOR_SENSOR_PIN } })
      .registerOnLeavingRoom([]() {
        room_client->sendPostPlainRequest("/open-door", "");
      }));
  delay(1000);
  room_lcd_controller->updateDisplay("Room sensors...", "Done!");

  delay(500);
  room_lcd_controller->updateDisplay("Smart bulb...", "");
  lamp_controller = std::make_unique<RoomLampController>();
  lamp_controller->connectToYeelight();
  delay(1000);
  room_lcd_controller->updateDisplay("Smart bulb...", "Done!");

  delay(500);
  room_lcd_controller->updateDisplay("Main server...", "");
  room_server = std::make_unique<RoomServer>();
  room_server->registerGetHandler("/", []() -> std::string {
    return std::string(HTMLMainPage::HTML);
  });


  room_server->registerPostPlainHandler("/auth", [room_lcd_controller](std::string const& user_id) {
    room_lcd_controller->updateDisplayExternalEvent(std::string("Welcome, "), user_id);
    Serial.println(user_id.c_str());
  });
  room_server->registerPostPlainHandler("/password-reminder", [room_lcd_controller](std::string const& data) {
    room_lcd_controller->updateDisplayExternalEvent("Enter password", data);
  });
  room_server->registerPostPlainHandler("/2fa", [room_lcd_controller](std::string const& user_id) {
    room_lcd_controller->updateDisplayExternalEvent(std::string("Enter the room, "), user_id);
    Serial.println(user_id.c_str());
  });
  room_server->registerWebSocketHandler("water_level", [](std::string x) {
    Serial.println(x.c_str());
    Serial.println("water_level");
    room_server->broadcastViaWebsocket(x);
  });
  room_server->connect();
  delay(1000);
  room_lcd_controller->updateDisplay("Main server...", "Done!");

  delay(500);
  room_lcd_controller->updateDisplay("Main server IP:", std::string(WiFi.localIP().toString().c_str()));
  delay(3000);

  room_lcd_controller->updateDisplay("Welcome to the", "Smart Room!");
}


void loop() {
  delay(10);
  room_server->handleTick();
  room_environment->handleTick();
  room_lcd_controller->handleTick();

  lamp_controller->controlLamp(room_environment->getPeopleInRoomCount(), room_environment->getIsNoMovementMissingForTime());

  StaticJsonDocument<200> event_data;
  event_data["controller_id"] = "main-controller";
  event_data["people_in_room_cnt"] = room_environment->getPeopleInRoomCount();
  event_data["no_movement_in_room"] = room_environment->getIsNoMovementMissingForTime();

  StaticJsonDocument<200> doc;
  doc["event"] = "room_update";
  doc["event_data"] = event_data;

  String jsonMessage;
  serializeJson(doc, jsonMessage);

  room_server->sampledBroadcastViaWebsocket(std::string(jsonMessage.c_str()));
}
