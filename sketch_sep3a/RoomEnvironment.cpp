#include "RoomEnvironment.hpp"

RoomEnvironment::RoomEnvironment(RoomMotionSensor& room_motion_sensor, RoomOccupancyTracker& room_occupancy_controller)
  : room_motion_sensor(room_motion_sensor), room_occupancy_controller(room_occupancy_controller) {}

RoomEnvironment::RoomEnvironment(RoomMotionSensor&& room_motion_sensor, RoomOccupancyTracker&& room_occupancy_controller)
  : room_motion_sensor(std::move(room_motion_sensor)), room_occupancy_controller(std::move(room_occupancy_controller)) {}

void RoomEnvironment::handleTick() {
  room_motion_sensor.handleTick();
  room_occupancy_controller.handleTick();
}

int RoomEnvironment::getPeopleInRoomCount() const {
  return room_occupancy_controller.getPeopleInRoom();
}

bool RoomEnvironment::getIsNoMovementMissingForTime() const {
  return room_motion_sensor.hasMotionExpired();
}
