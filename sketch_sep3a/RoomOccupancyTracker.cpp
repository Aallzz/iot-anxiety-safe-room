#include "RoomOccupancyTracker.hpp"
#include <Arduino.h>

RoomOccupancyTracker::RoomOccupancyTracker(RoomDoorwaySensor const& doorway_sensor)
  : doorway_sensor(doorway_sensor) {}

RoomOccupancyTracker::RoomOccupancyTracker(RoomDoorwaySensor&& doorway_sensor)
  : doorway_sensor(std::move(doorway_sensor)) {}

void RoomOccupancyTracker::handleTick() {
  doorway_sensor.handleTick();

  if (doorway_sensor.isRoomEnteredMoment()) {
    updateOccupancy(true);
  }

  if (doorway_sensor.isRoomLeavingMoment()) {
    updateOccupancy(false);
    onLeavingRoom();
  }
}

RoomOccupancyTracker RoomOccupancyTracker::registerOnLeavingRoom(std::function<void()> callback) {
  onLeavingRoom = callback;
  return *this;
}

int RoomOccupancyTracker::getPeopleInRoom() const {
  return peopleInRoom;
}

void RoomOccupancyTracker::updateOccupancy(bool entering) {
  if (entering) {
    peopleInRoom++;
  } else if (peopleInRoom > 0) {
    peopleInRoom--;
  }
}