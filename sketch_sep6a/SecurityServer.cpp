#include "SecurityServer.hpp"
#include <Arduino.h>

SecurityServer::SecurityServer() {
  if (!MDNS.begin("security-controller")) {  // security-controller.local
    Serial.println("Error starting mDNS");
    return;
  }
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS responder started");
}

void SecurityServer::connect() {
  Serial.println("Start server");

  server.on("/", [this]() {
    this->get_index();
  });

  for (const auto& handler : get_request_handlers) {
    server.on(handler.first.c_str(), HTTP_GET, handler.second);
  }

  for (const auto& handler : post_plain_request_handlers) {
    server.on(handler.first.c_str(), HTTP_POST, wrap_handle_post_plain_request(handler.second));
  }

  server.begin();
}

void SecurityServer::registerGetHandler(const std::string& route, std::function<void()> handler) {
  get_request_handlers[route] = handler;
}

void SecurityServer::registerPostPlainHandler(const std::string& route, std::function<void(std::string)> handler) {
  post_plain_request_handlers[route] = handler;
}

void SecurityServer::handleTick() {
  server.handleClient();
  MDNS.update();
}

std::function<void()> SecurityServer::wrap_handle_post_plain_request(std::function<void(std::string)> handler) {
  return [this, handler]() {
    if (!this->server.hasArg("plain")) {
      this->server.send(400, "text/plain", "Invalid request");
    }

    std::string data{ server.arg("plain").c_str() };

    Serial.println("data");
    Serial.println(this->server.arg("plain"));
    Serial.println(data.c_str());
    handler(data);
  };
}

void SecurityServer::get_index() {
  String html = "<!DOCTYPE html> <html>";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The Smart Door Dashboard</h1>";
  html += "</body> </html>";

  server.send(200, "text/html", html);
}
