#include "AuthentificationSensor.hpp"

AuthentificationSensor::AuthentificationSensor(uint8_t sda_pin, uint8_t rst_pin)
  : sda_pin(sda_pin), rst_pin(rst_pin), rfid(sda_pin, rst_pin) {
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();
}

void AuthentificationSensor::handleTick() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("RFID Detected");
    readAndStoreUserID();
  }
  rfid.PICC_HaltA();
}

long AuthentificationSensor::getLastReadTime() const {
  return last_read_timestamp;
}

std::array<byte, 4> AuthentificationSensor::getLastReadUserID() const {
  return last_read_user_id;
}

std::string AuthentificationSensor::getLastReadUserIDAsString() const {
  std::string user_id;
  for (int i = 0; i < last_read_user_id.size(); ++i) {
    user_id += std::string(String(last_read_user_id[i], HEX).c_str());
  }
  return user_id;
}

void AuthentificationSensor::readAndStoreUserID() {
  last_read_timestamp = millis();
  for (size_t i = 0; i < last_read_user_id.size(); ++i) {
    last_read_user_id[i] = rfid.uid.uidByte[i];
  }
}
