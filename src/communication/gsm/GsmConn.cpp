#include "communication/gsm/GsmConn.h"

int GsmConn::start() {}

void GsmConn::stop() {}

bool GsmConn::isConnected() {
  return false;
}

bool GsmConn::isOk() {
  return false;
}

Http::Response GsmConn::httpGet(const String& url, String& authorization) {}

Http::Response GsmConn::httpPost(const String& url, String& body, String& authorization) {}

Http::Response GsmConn::httpPut(const String& url, String& body, String& authorization) {}

void GsmConn::setAuthorizationHeader(String& value) {}
