#include "RoomDoorwaySensor.hpp"

RoomDoorwaySensor::RoomDoorwaySensor(
  uint8_t ultrasonic_sensor_trigger,
  uint8_t ultrasonic_sesnor_echo_pin,
  uint8_t laster_emitter_pin,
  uint8_t phototransistor_sensor_pin)
  : ultrasonic_sensor_trigger(ultrasonic_sensor_trigger),
    ultrasonic_sesnor_echo_pin(ultrasonic_sesnor_echo_pin),
    laster_emitter_pin(laster_emitter_pin),
    phototransistor_sensor_pin(phototransistor_sensor_pin) {
  pinMode(ultrasonic_sensor_trigger, OUTPUT);
  pinMode(ultrasonic_sesnor_echo_pin, INPUT);

  pinMode(laster_emitter_pin, OUTPUT);
  analogWrite(laster_emitter_pin, 127);  // Modulate the laser at 50% duty cycle (around 490 Hz)

  pinMode(phototransistor_sensor_pin, INPUT);
}

void RoomDoorwaySensor::handleTick() {
  updateDistance();
  updateCrossingState();
  // Serial.print(previus_object_distance);
  // Serial.print("\t");
  // Serial.println(current_object_distance);
}

bool RoomDoorwaySensor::isRoomEnteredMoment() const {
  return crossing_state == "FINISHED" && current_object_distance < start_object_distance;
}

bool RoomDoorwaySensor::isRoomLeavingMoment() const {
  return crossing_state == "FINISHED" && current_object_distance > start_object_distance;
}

void RoomDoorwaySensor::updateCrossingState() {
  if (isLaserInterrupted()) {
    if (crossing_state == "START") {
      crossing_state = "IN_PROGRESS";
    }
    if (crossing_state == "NONE") {
      crossing_state = "START";
      start_object_distance = current_object_distance;
    }
    last_crossing_timestamp = millis();
  } else {
    if (crossing_state == "FINISHED") {
      crossing_state = "NONE";
    }
  }

  if (crossing_state == "IN_PROGRESS" && last_crossing_timestamp < millis() - CROSSING_UPDATE_TO_FINISHED_FROM_IN_PROGRESS_DELAY) {
    crossing_state = "FINISHED";
  }
}

void RoomDoorwaySensor::updateDistance() {
  if (last_distance_update_happened_timestamp > millis() - OBJECT_DISTANCE_UPDATE_DELAY) {
    return;
  }
  previus_object_distance = current_object_distance;
  current_object_distance = measureDistanceInCentimeters();
  last_distance_update_happened_timestamp = millis();
}

long RoomDoorwaySensor::measureDistanceInCentimeters() {
  digitalWrite(ultrasonic_sensor_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonic_sensor_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonic_sensor_trigger, LOW);

  long duration = pulseIn(ultrasonic_sesnor_echo_pin, HIGH);

  int distance = duration * 0.034 / 2 * 10;

  return distance;
}

bool RoomDoorwaySensor::isLaserInterrupted() const {
  int phototransistor_value = analogRead(phototransistor_sensor_pin);
  return phototransistor_value > PHOTO_SENSOR_ACTIVATION_THRESHOLD;
}

bool RoomDoorwaySensor::isObjectApproaching() const {
  return previus_object_distance > 0 && current_object_distance > 0 && current_object_distance < previus_object_distance;
}

bool RoomDoorwaySensor::isObjectLeaving() const {
  return previus_object_distance > 0 && current_object_distance > 0 && current_object_distance > previus_object_distance;
}
