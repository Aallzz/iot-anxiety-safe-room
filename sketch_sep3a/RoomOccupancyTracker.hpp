#pragma once

#include "RoomDoorwaySensor.hpp"
#include <string>
#include <functional>

struct RoomOccupancyTracker {
  RoomOccupancyTracker(RoomDoorwaySensor const& doorway_sensor);
  RoomOccupancyTracker(RoomDoorwaySensor&& doorway_sensor);

  void handleTick();

  int getPeopleInRoom() const;

  RoomOccupancyTracker registerOnLeavingRoom(std::function<void()>);

private:
  void updateOccupancy(bool entering);

  int peopleInRoom{};

  RoomDoorwaySensor doorway_sensor;

  std::function<void()> onLeavingRoom;

  void sendRequestToMainController(const std::string& endpoint, const std::string& data);
};
