#pragma once

#include "communication/common/Http.h"
#include <Arduino.h>

class GsmConn {
 public:
  static int start();
  static void stop();
  static Http::Response httpGet(const String& url, String& authorization);
  static Http::Response httpPost(const String& url, String& body, String& authorization);
  static Http::Response httpPut(const String& url, String& body, String& authorization);
  static void setAuthorizationHeader(String& value);
  static bool isConnected();
  static bool isOk();
};