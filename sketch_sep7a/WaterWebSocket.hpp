#pragma once

#include <WebSocketsClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <optional>
#include <IPAddress.h>
#include <string>

class WaterWebSocket {
public:
  WaterWebSocket();

  void connect();
  void broadcastViaWebsocket(std::string const& data);
  void handleTick();

  void registerWebSocketHandler(const std::string& event, std::function<void(std::string)> handler);
  

private:
  void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);
  std::optional<IPAddress> findWebsocketServiceIPForName(std::string const& mdns_hostname);

  std::map<std::string, std::function<void(std::string)>> websocket_event_handlers;

  WebSocketsClient websocket;
};