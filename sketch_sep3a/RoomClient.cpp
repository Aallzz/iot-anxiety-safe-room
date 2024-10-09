#include "RoomClient.hpp"
#include <Arduino.h>

RoomClient::RoomClient(const std::string& mdns_hostname, const std::string& service_type, const std::string& protocol)
  : mdns_hostname(mdns_hostname), service_type(service_type), protocol(protocol) {
}

std::optional<IPAddress> RoomClient::findServiceIP() {
  int services_count = MDNS.queryService(service_type.c_str(), protocol.c_str());

  for (int service_index = 0; service_index < services_count; ++service_index) {
    if (MDNS.hostname(service_index) == String(mdns_hostname.c_str())) {
      return MDNS.IP(service_index);
    }
  }

  return std::nullopt;
}

void RoomClient::connectToService(const String& ip) {
  if (!connected) {
    client.onConnect([this](void* arg, AsyncClient* client) {
      Serial.println("Connected to service!");
      connected = true;
    });

    client.onDisconnect([this](void* arg, AsyncClient* client) {
      Serial.println("Disconnected from service.");
      connected = false;
    });

    client.connect(ip.c_str(), 80);
    delay(100);
  }
}

bool RoomClient::sendPostPlainRequest(const String& endpoint, const String& data) {
  std::optional<IPAddress> service_ip = findServiceIP();

  if (!service_ip.has_value()) {
    Serial.println("Service not found via mDNS");
    return false;
  }

  String ip_string = service_ip.value().toString();
  connectToService(ip_string);

  if (!connected) {
    Serial.println("Failed to connect to service.");
    return false;
  }

  String postRequest = String("POST ") + endpoint + " HTTP/1.1\r\n" + "Host: " + ip_string + "\r\n" + "Content-Type: text/plain\r\n" + "Content-Length: " + String(data.length()) + "\r\n" + "\r\n" + data;

  client.write(postRequest.c_str());

  closeConnection();

  return true;
}

void RoomClient::closeConnection() {
  if (connected) {
    client.close();
    connected = false;
    Serial.println("Connection closed.");
  }
}
