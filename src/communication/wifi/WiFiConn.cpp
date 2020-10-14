#include "communication/wifi/WiFiConn.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "maintenance/Guardian.h"

static HTTPClient http;
bool WiFiConn::connected = false;

int WiFiConn::start() {
  if (Config::getWifiSsid().equals("")) {
    Logger::info("[WiFiConn::start()] WiFi network is not set!");
    connected = false;
    return -1;
  }

  WiFi.begin(Config::getWifiSsid().c_str(), Config::getWifiPassword().c_str());
  Logger::info(("[WiFiConn::start()] Connecting to wifi " + Config::getWifiSsid()).c_str());

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Logger::info("[WiFiConn::start()] Connecting to wifi...");
    i++;
    if (i > 8) {
      Logger::error("[WiFiConn::start()] WiFi - error!");
      connected = false;
      return -1;
    }
    delay(1000);
  }

  // delay because sometimes WiFi still
  // doesn't work and throws DNS error when connecting
  delay(1000);

  Logger::info("[WiFiConn::start()] WiFi - connected!");
  connected = true;
  return 0;
}

void WiFiConn::stop() {
  WiFi.disconnect();
  connected = false;
  delay(500);
  Logger::info("[WiFiConn::stop()] WiFi - disconnected");
}

bool WiFiConn::isConnected() {
  return WiFiConn::connected;
}


bool WiFiConn::isOk() {
  if(!WiFiConn::connected) return false;
  if(WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return Ping.ping("www.google.com");
}

Http::Response WiFiConn::httpGet(const String& url, String& authorization) {
  Guardian::checkWiFiConnection();
  Logger::debug(("GET Request to url: " + url).c_str());

  http.begin(url);
  if (!authorization.equals("")) {
    http.addHeader("Authorization", authorization);
  }

  Http::Response response;
  response.code = http.GET();
  response.payload = http.getString();
  http.end();
  return response;
}

Http::Response WiFiConn::httpPost(const String& url, String& body, String& authorization) {
  Guardian::checkWiFiConnection();
  Logger::debug(("POST Request to url: " + url + " with body: " + body).c_str());
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "Airella");
  if (!authorization.equals("")) {
    http.addHeader("Authorization", authorization);
  }

  Http::Response response;
  response.code = http.POST(body);
  response.payload = http.getString();
  http.end();
  return response;
}

Http::Response WiFiConn::httpPut(const String& url, String& json, String& authorization) {
  Guardian::checkWiFiConnection();
  Logger::debug(("PUT Request to url: " + url + " with body: " + json).c_str());

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "Airella");
  if (!authorization.equals("")) {
    http.addHeader("Authorization", authorization);
  }

  Http::Response response;
  response.code = http.PUT(json);
  response.payload = http.getString();
  http.end();
  return response;
}