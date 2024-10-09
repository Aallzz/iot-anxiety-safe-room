// This file contains all the logic for setup the node for security of the 
// room's door. The security itself is done using RFID sensor using RFID_SDA_PIN and RFID_RST_PIN
// and password buttons PASSWORD_BUTTON_1_PIN and PASSWORD_BUTTON_2_PIN, together they
// represent sort of 2FA. If it's passed than servomotor SERVO_PIN represent the door lock, that is getting unlocked
// for a limited time.

// The node communiated with the room node (main controller) the information about auth progress and sends reminders
// about the limited time to input the password to unlock the door.

// The node also recieves the communication from room node to unlock the door

#include <memory>
#include <string>
#include <string_view>
#include <functional>

// Util class like everywhere else to connect to a wifi
#include "SecurityWifiConnection.hpp"

// Server class that allows to set how to handle post requests 
// using lamba handlers, in the context of the project 
// handles open door request from main node
#include "SecurityServer.hpp"

// Client class that allows sending post HTTP requests
// to server of choice
#include "SecurityClient.hpp"

// Auth process is fairlly compliacated and also for the node
// it required to store some state (like when was the last read)
// therefore Auth sensor is a standalone class for sensor
#include "AuthentificationSensor.hpp"

// Class for handling password state using sensory data from password pins
// Similar to Auth sensor there is a state involved (how much of the password is entered)
// therefore a separate class
#include "PasswordSensor.hpp"

// Controller class for servo control using parameters like 2FA result and password correctness
#include "ServoController.hpp"

// Class collecting all sensory information for the node and exposing important data like 
// 2FA pass resuult 
#include "SecurityEnvironment.hpp"

constexpr uint8_t RFID_SDA_PIN = D4;
constexpr uint8_t RFID_RST_PIN = D3;
constexpr uint8_t PASSWORD_BUTTON_1_PIN = D2;
constexpr uint8_t PASSWORD_BUTTON_2_PIN = D8;
constexpr uint8_t SERVO_PIN = D1;

constexpr std::string_view MAIN_SERVER_NAME = "main-controller.local";

SecurityWifiConnection security_wifi_connection;
std::unique_ptr<SecurityServer> security_server;
std::unique_ptr<SecurityClient> security_client;
std::unique_ptr<SecurityEnvironment> security_environment;
std::unique_ptr<ServoController> servo_controller;

void setup() {
  Serial.begin(19200);

  security_wifi_connection.connect();

  security_server = std::make_unique<SecurityServer>();
  security_server->registerPostPlainHandler("/open-door", [servo_controller](std::string const& dara) {
    servo_controller->controlServo(true);
  });
  security_server->connect();

  security_client = std::make_unique<SecurityClient>(std::string(MAIN_SERVER_NAME));

  security_environment = std::make_unique<SecurityEnvironment>(
    AuthentificationSensor(RFID_SDA_PIN, RFID_RST_PIN),
    PasswordSensor({ PASSWORD_BUTTON_1_PIN, PASSWORD_BUTTON_2_PIN }));
  security_environment->registerOnEveryTick([](std::string rfid, std::string password) {
    // Serial.print(rfid.c_str()); Serial.print("+++"); Serial.print(password.c_str()); Serial.println(" ");
  });
  security_environment->registerOnNewKnownUser([](std::string user_id) {
    security_client->sendPostPlainRequest("/auth", String(user_id.c_str()));
  });
  security_environment->registerOnEvery3SecondsSinceAuth([](long time_left) {
    security_client->sendPostPlainRequest("/password-reminder", String(time_left / 1000) + " seconds left");
  });
  security_environment->registerOnUserPassed2FA([]() {
    security_client->sendPostPlainRequest("/2fa", "PASSED");
  });
  security_environment->registerOnNewUnknownUser([](std::string user_id) {
    Serial.println("Failed");
  });
  security_environment->registerOnTimeExpirationSinceAuthCallback([]() {
    Serial.println("Expired");
  });

  servo_controller = std::make_unique<ServoController>(SERVO_PIN);
}

void loop() {
  delay(10);
  security_server->handleTick();
  security_environment->handleTick();
  servo_controller->handleTick();

  servo_controller->controlServo(security_environment->isUserPassed2FA());
}