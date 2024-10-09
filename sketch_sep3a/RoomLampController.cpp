#include "RoomLampController.hpp"
#include <ESP8266Ping.h>

RoomLampController::RoomLampController() {
  lamp = std::make_unique<Yeelight>();
}

void RoomLampController::connectToYeelight() {
  if (isLampReachable()) {
    lamp->connect(lampIP.data(), lampPort);

    if (lamp->is_connected()) {
      Serial.println("Connected to the Yeelight device");
    } else {
      Serial.println("Connection error");
    }
  }
}

void RoomLampController::controlLamp(int peopleInRoom, bool hasMotionExpired) {
  if (peopleInRoom > 0 && hasMotionExpired && !lightIsOn) {
    return;
  }
  if (peopleInRoom > 0 && !lightIsOn) {
    turnOn();
  } else if (lightIsOn && (peopleInRoom <= 0 || hasMotionExpired)) {
    turnOff();
  }
}

bool RoomLampController::isLampReachable() const {
  IPAddress lampIPAddress(lampIP[0], lampIP[1], lampIP[2], lampIP[3]);
  if (Ping.ping(lampIPAddress)) {
    Serial.println("Yeelight device is reachable!");
    return true;
  } else {
    Serial.println("Yeelight device is not reachable!");
    return false;
  }
}

void RoomLampController::turnOn() {
  if (!lightIsOn && lamp->is_connected()) {
    lamp->turn_on();
    lightIsOn = true;
    Serial.println("Lamp turned on");
  }
}

void RoomLampController::turnOff() {
  if (lightIsOn && lamp->is_connected()) {
    lamp->turn_off();
    lightIsOn = false;
    Serial.println("Lamp turned off");
  }
}
