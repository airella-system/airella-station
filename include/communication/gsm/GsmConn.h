#pragma once

#include <Arduino.h>
#include "communication/common/Http.h"
#include "communication/gsm/GSM.h"

class GSM;

class GsmConn {
 public:
  static int start();
  static void stop();
  static Http::Response httpGet(const String& url, const String& authorization);
  static Http::Response httpPost(const String& url, const String body, const String& authorization);
  static Http::Response httpPut(const String& url, const String body, const String& authorization);
  static bool isConnected();
  static bool isOk();
  static GSM gsm;

 private:
  static Http::Response buildRequest(
    const String& url, 
    Http::Method method, 
    const String& authorization,
    const String* body = NULL
  );
};
