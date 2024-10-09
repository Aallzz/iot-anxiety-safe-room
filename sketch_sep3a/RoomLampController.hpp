#pragma once

#include <Yeelight.h>
#include <Arduino.h>
#include <Ping.h>
#include <array>
#include <memory>

struct RoomLampController {

  RoomLampController();
  void connectToYeelight();
  void controlLamp(int peopleInRoom, bool hasMotionExpired);

private:
  bool isLampReachable() const;
  void turnOn();
  void turnOff();

  std::unique_ptr<Yeelight> lamp{};
  bool lightIsOn{ false };

  static constexpr std::array<uint8_t, 4> lampIP = { 192, 168, 0, 4 };
  static constexpr uint16_t lampPort = 55443;
};