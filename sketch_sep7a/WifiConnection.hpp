#pragma once

#include <ESP8266WiFi.h>
#include <string_view>

struct WifiConnection {
  void connect();

private:
  static constexpr std::string_view ssid = "SKYKFCCI";
  static constexpr std::string_view password = "hNIaa14dDwXf";
};
