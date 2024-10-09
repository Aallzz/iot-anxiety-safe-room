#pragma once

#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <array>
#include <string>

struct AuthentificationSensor {
  AuthentificationSensor(uint8_t sda_pin, uint8_t rst_pin);

  void handleTick();

  long getLastReadTime() const;

  std::array<byte, 4> getLastReadUserID() const;

  std::string getLastReadUserIDAsString() const;

private:
  void readAndStoreUserID();

  uint8_t sda_pin;
  uint8_t rst_pin;

  MFRC522 rfid;

  long last_read_timestamp{};
  std::array<byte, 4> last_read_user_id{};
};
