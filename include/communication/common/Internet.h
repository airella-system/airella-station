#pragma once

#include <Arduino.h>
#include "communication/common/Http.h"
#include "communication/gsm/GsmConn.h"
#include "communication/wifi/WiFiConn.h"
#include "config/Config.h"

class Internet {
 public:
  enum Type { WIFI = 0, GSM = 1 };

  static void setType(Type type);
  static int start();
  static void stop();
  static bool isConnected();
  static Http::Response httpGet(String& url, String authorization = "");
  static Http::Response httpPost(String& url, String& body);
  static Http::Response httpPost(String& url, String& body, String& authorization);
  static Http::Response httpPut(String& url, String& body, String authorization = "");

 private:
  static Type type;
};