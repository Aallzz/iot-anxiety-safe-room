#include "WaterWebSocket.hpp"

#include <Arduino.h>
#include <ArduinoJson.h>

WaterWebSocket::WaterWebSocket() {
  if (!MDNS.begin("water-controller")) {
    Serial.println("Error starting mDNS");
  }
}

void WaterWebSocket::connect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: no WiFi connection");
    return;
  }

  websocket.onEvent([this](WStype_t type, uint8_t* payload, size_t length) {
    this->webSocketEvent(type, payload, length);
  });

  std::optional<IPAddress> main_controller_ip = findWebsocketServiceIPForName("main-controller.local");

  if (!main_controller_ip) {
    Serial.println("Error: could not detect main-controller");
    return;
  }

  websocket.begin(main_controller_ip.value().toString(), 80, "/ws");
}

void WaterWebSocket::broadcastViaWebsocket(std::string const& data) {
  String string_data = String(data.c_str());
  websocket.sendTXT(string_data);
}

void WaterWebSocket::handleTick() {
  websocket.loop();
}

void WaterWebSocket::registerWebSocketHandler(const std::string& event, std::function<void(std::string)> handler) {
  websocket_event_handlers[event] = handler;
}

void WaterWebSocket::webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
    case WStype_CONNECTED:
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    case WStype_TEXT:
      payload[length] = 0;
      String message = (char*)payload;

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, message);

      if (error) {
        Serial.println("Failed to parse JSON from WebSocket message");
        return;
      }


      std::string event = std::string(doc["event"]);
      if (!websocket_event_handlers.count(event)) {
        return;
      }
      websocket_event_handlers[event](doc["event_data"]);
  }
}

std::optional<IPAddress> WaterWebSocket::findWebsocketServiceIPForName(std::string const& mdns_hostname) {
  int services_count = MDNS.queryService("ws", "tcp");

  for (int service_index = 0; service_index < services_count; ++service_index) {
    if (MDNS.hostname(service_index) == String(mdns_hostname.c_str())) {
      return MDNS.IP(service_index);
    }
  }

  return {};
}