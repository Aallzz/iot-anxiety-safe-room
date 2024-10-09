#include "RoomServer.hpp"
#include <Arduino.h>

RoomServer::RoomServer() {
  if (!MDNS.begin("main-controller")) {  // main-controller.local
    Serial.println("Error starting mDNS");
  }
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 80);
}

void RoomServer::connect() {
  Serial.println("Start");

  ws.onEvent([this](
               AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    this->onWebSocketEvent(server, client, type, arg, data, len);
  });
  server.addHandler(&ws);

  for (const auto& handler : get_request_handlers) {
    server.on(handler.first.c_str(), HTTP_GET, wrap_handle_get_request(handler.second));
  }

  for (const auto& handler : post_plain_request_handlers) {
    server.on(
      handler.first.c_str(), HTTP_POST, [](AsyncWebServerRequest* request) {}, nullptr,
      wrap_handle_post_plain_request(handler.second));
  }

  server.begin();
}

void RoomServer::registerGetHandler(const std::string& route, std::function<std::string()> handler) {
  get_request_handlers[route] = handler;
}

void RoomServer::registerPostPlainHandler(const std::string& route, std::function<void(std::string)> handler) {
  post_plain_request_handlers[route] = handler;
}

void RoomServer::registerWebSocketHandler(const std::string& event, std::function<void(std::string)> handler) {
  websocket_event_handlers[event] = handler;
}

void RoomServer::broadcastViaWebsocket(const std::string& data) {
  ws.textAll(String(data.c_str()));
}

void RoomServer::sampledBroadcastViaWebsocket(const std::string& data) {
  if (millis() - last_sampled_broadcast_timestamp < 1000) {
    return;
  }
  last_sampled_broadcast_timestamp = millis();
  ws.textAll(String(data.c_str()));
}

void RoomServer::handleTick() {
  ws.cleanupClients();
  MDNS.update();
}

std::function<void(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total)>
RoomServer::wrap_handle_post_plain_request(std::function<void(std::string)> handler) {
  return [this, handler](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    static std::string body;
    body.append((char*)data, len);

    if (index + len == total) {
      Serial.println("Full POST data received:");
      Serial.println(body.c_str());
      handler(body);
      body.clear();
    }
  };
}

std::function<void(AsyncWebServerRequest* request)>
RoomServer::wrap_handle_get_request(std::function<std::string()> handler) {
  return [this, handler](AsyncWebServerRequest* request) {
    std::string html = handler();
    return request->send(200, "text/html", String(html.c_str()));
  };
}

void RoomServer::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client %u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client %u disconnected. Reason: %s\n", client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DATA) {
    data[len] = 0;
    String message = (char*)data;

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.println("Failed to parse JSON from WebSocket message");
      return;
    }

    std::string event = std::string(doc["event"]);
    websocket_event_handlers[event](std::string((char*)data));
  }
}
