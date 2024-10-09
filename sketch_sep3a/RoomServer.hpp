#pragma once

#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <map>
#include <string>
#include <functional>

struct RoomServer {
  RoomServer();
  void connect();

  void registerGetHandler(const std::string& route, std::function<std::string()> handler);
  void registerPostPlainHandler(const std::string& route, std::function<void(std::string)> handler);
  void registerWebSocketHandler(const std::string& event, std::function<void(std::string)> handler);

  void broadcastViaWebsocket(const std::string& data);
  void sampledBroadcastViaWebsocket(const std::string& data);

  void handleTick();

private:
  std::function<void(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total)>
  wrap_handle_post_plain_request(std::function<void(std::string)> handler);

  std::function<void(AsyncWebServerRequest* request)> wrap_handle_get_request(std::function<std::string()> handler);

  void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);

  long last_sampled_broadcast_timestamp {};

  std::map<std::string, std::function<std::string()>> get_request_handlers;
  std::map<std::string, std::function<void(std::string)>> post_plain_request_handlers;
  std::map<std::string, std::function<void(std::string)>> websocket_event_handlers;

  AsyncWebServer server{ 80 };
  AsyncWebSocket ws{ "/ws" };
};
