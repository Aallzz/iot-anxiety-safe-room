#include "WifiConnection.hpp"

void WifiConnection::connect() {
    

  WiFi.begin(ssid.data(), password.data());

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}