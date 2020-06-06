#pragma once

#include "communication/common/Http.h"
#include "communication/gsm/GsmConn.h"
#include "communication/wifi/WiFiConn.h"
#include <Arduino.h>

class Internet {

public:
  enum Type { WIFI = 0, GSM = 1 };

  static void setType(Type type);
  static int start();
  static void stop();
  static Http::Response httpGet(String url);
  static Http::Response httpPost(String url, String json);
  static Http::Response httpPut(String url, String json);
  static void setAuthorizationHeader(String value);

private:
  static Type type;
};