#pragma once
#include <Arduino.h>
#include "communication/common/Http.h"

class WiFiConn {
 public:
  static int start();
  static void stop();
  static bool isConnected();
  static Http::Response httpGet(String url, String authorization = "");
  static Http::Response httpPost(String url, String body, String authorization = "");
  static Http::Response httpPut(String url, String json, String authorization = "");

 private:
  static bool connected;
};