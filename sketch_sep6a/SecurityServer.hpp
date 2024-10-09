#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <map>
#include <string>
#include <functional>

struct SecurityServer {
  SecurityServer();
  void connect();
  void registerGetHandler(const std::string& route, std::function<void()> handler);
  void registerPostPlainHandler(const std::string& route, std::function<void(std::string)> handler);
  void handleTick();

private:
  std::function<void()> wrap_handle_post_plain_request(std::function<void(std::string)> handler);
  void get_index();

  std::map<std::string, std::function<void()>> get_request_handlers;
  std::map<std::string, std::function<void(std::string)>> post_plain_request_handlers;

  ESP8266WebServer server{ 80 };
};
