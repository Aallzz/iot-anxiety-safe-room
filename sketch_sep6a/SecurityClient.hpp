#pragma once

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <optional>
#include <IPAddress.h>
#include <string>

class SecurityClient {
public:
  SecurityClient(const std::string& mdns_hostname, const std::string& service_type = "http", const std::string& protocol = "tcp");

  bool sendPostPlainRequest(const String& endpoint, const String& data);

private:
  std::optional<IPAddress> findServiceIP();
  void connectToService(const String& ip);
  void closeConnection();

  AsyncClient client;
  bool connected{ false };

  std::string mdns_hostname;
  std::string service_type;
  std::string protocol;
};