#pragma once

#include <Arduino.h>
#include <string>

class RoomDoorwaySensor {
public:
  RoomDoorwaySensor(
    uint8_t ultrasonic_sensor_trigger,
    uint8_t ultrasonic_sesnor_echo_pin,
    uint8_t laster_emitter_pin,
    uint8_t phototransistor_sensor_pin);

  void handleTick();

  bool isRoomEnteredMoment() const;
  bool isRoomLeavingMoment() const;

  static constexpr int PHOTO_SENSOR_ACTIVATION_THRESHOLD = 100;
  static constexpr int CROSSING_UPDATE_TO_FINISHED_FROM_IN_PROGRESS_DELAY = 1000;  // ms
  static constexpr int OBJECT_DISTANCE_UPDATE_DELAY = 50;                          // ms

private:
  void updateCrossingState();
  void updateDistance();
  long measureDistanceInCentimeters();
  bool isLaserInterrupted() const;
  bool isObjectApproaching() const;
  bool isObjectLeaving() const;

  long previus_object_distance{};
  long current_object_distance{};
  long last_distance_update_happened_timestamp{};

  long start_object_distance{};

  std::string crossing_state{ "NONE" };
  long last_crossing_timestamp{ 0 };


  uint8_t ultrasonic_sensor_trigger;
  uint8_t ultrasonic_sesnor_echo_pin;
  uint8_t laster_emitter_pin;
  uint8_t phototransistor_sensor_pin;
};
