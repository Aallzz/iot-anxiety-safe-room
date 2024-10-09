#pragma once

#include "RoomMotionSensor.hpp"
#include "RoomOccupancyTracker.hpp"

struct RoomEnvironment {
  RoomEnvironment(RoomMotionSensor& room_motion_sensor, RoomOccupancyTracker& room_occupancy_controller);
  RoomEnvironment(RoomMotionSensor&& room_motion_sensor, RoomOccupancyTracker&& room_occupancy_controller);

  void handleTick();
  int getPeopleInRoomCount() const;
  bool getIsNoMovementMissingForTime() const;

private:
  RoomMotionSensor room_motion_sensor;
  RoomOccupancyTracker room_occupancy_controller;
};
