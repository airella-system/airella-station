#pragma once

#include <Arduino.h>
#include "communication/common/Http.h"
#include "communication/gsm/GSM.h"
#include "communication/gsm/GsmConn.h"
#include "communication/wifi/WiFiConn.h"
#include "config/Config.h"
#include "maintenance/Statistics.h"

class Internet {
 public:
  enum Type { None = -1, WIFI = 0, GSM = 1 };

  static int resetType(Type type);
  static int start();
  static void stop();
  static bool isConnected();
  static bool isOk();
  static Http::Response httpGet(const String& url);
  static Http::Response httpGet(const String& url, String& authorization);
  static Http::Response httpPost(const String& url, String& body);
  static Http::Response httpPost(const String& url, String& body, String& authorization);
  static Http::Response httpPut(const String& url, String& body, String& authorization);

 private:
  static Type type;
};