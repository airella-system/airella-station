#include "communication/gsm/GsmConn.h"

int GsmConn::start() {}

void GsmConn::stop() {}

bool GsmConn::isConnected() {
  return false;
}

Http::Response GsmConn::httpGet(String url) {}

Http::Response GsmConn::httpPost(String url, String json) {}

Http::Response GsmConn::httpPut(String url, String json) {}

void GsmConn::setAuthorizationHeader(String value) {}
