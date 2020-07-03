#include "communication/wifi/WiFiConn.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include <HTTPClient.h>
#include <WiFi.h>

static HTTPClient http;
bool WiFiConn::connected = false;

int WiFiConn::start() {
  if (Config::getWifiSsid().equals("")) {
    Serial.println("WiFi network is not set!");
    connected = false;
    return -1;
  }
  WiFi.begin(Config::getWifiSsid().c_str(),
             Config::getWifiPassword().c_str());
  Serial.println("Connecting to wifi...");
  Serial.println(Config::getWifiSsid());
  Serial.println(Config::getWifiPassword());

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    i++;
    if (i > 8) {
      Serial.println("");
      Serial.println("WiFi - error!");
      connected = false;
      return -1;
    }
    delay(1000);
  }

  // delay because sometimes WiFi still
  // doesn't work and throws DNS error when connecting
  delay(1000);

  Serial.println("WiFi - connected!");
  connected = true;
  return 0;
}

void WiFiConn::stop() {
  WiFi.disconnect();
  delay(500);
}

bool WiFiConn::isConnected() {
  return WiFiConn::connected;
}

Http::Response WiFiConn::httpGet(String url, String authorization) {}

Http::Response WiFiConn::httpPost(String url, String body, String authorization) {
  Logger::debug(
      ("POST Request to url: " + url + " with body: " + body).c_str());

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  if (!authorization.equals("")) {
    http.addHeader("Authorization", authorization);
  }

  Http::Response response;
  response.code = http.POST(body);
  response.payload = http.getString();
  http.end();
  return response;
}

Http::Response WiFiConn::httpPut(String url, String json, String authorization) {
  Logger::debug(("PUT Request to url: " + url + " with body: " + json).c_str());

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  if (!authorization.equals("")) {
    http.addHeader("Authorization", authorization);
  }

  Http::Response response;
  response.code = http.PUT(json);
  response.payload = http.getString();
  http.end();
  return response;
}