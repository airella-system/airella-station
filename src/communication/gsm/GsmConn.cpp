#include "communication/gsm/GsmConn.h"

int GsmConn::start() {}

void GsmConn::stop() {}

bool GsmConn::isConnected() {
  return false;
}

Http::Response GsmConn::httpGet(String url, String authorization) {}

Http::Response GsmConn::httpPost(String url, String body, String authorization) {}

Http::Response GsmConn::httpPut(String url, String body, String authorization) {}

void GsmConn::setAuthorizationHeader(String value) {}
