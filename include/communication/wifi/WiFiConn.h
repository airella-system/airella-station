#pragma once
#include <Arduino.h>
#include <ESP32Ping.h>
#include "communication/common/Http.h"

class WiFiConn {
 public:
  static int start();
  static void stop();
  static bool isConnected();
  static bool isOk();
  static Http::Response httpGet(const String& url, const String& authorization);
  static Http::Response httpPost(const String& url, const String& body, const String& authorization);
  static Http::Response httpPut(const String& url, const String& json, const String& authorization);

 private:
  static bool connected;
};