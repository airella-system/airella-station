#pragma once

#include <Arduino.h>
#include "communication/common/Http.h"
#include "communication/gsm/GSM.h"

class GSM;

class GsmConn {
public:
  static int start();
  static void stop();
  static Http::Response httpGet(const String& url, String& authorization);
  static Http::Response httpPost(const String& url, String body, String& authorization);
  static Http::Response httpPut(const String& url, String body, String& authorization);
  static bool isConnected();
  static bool isOk();
private:
  static GSM gsm;
  static Http::Response buildRequest(const String& url, Http::Method method, String& authorization, String* body = NULL);
};
