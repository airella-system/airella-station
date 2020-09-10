#include "communication/common/Internet.h"

Internet::Type Internet::type = Internet::WIFI;

int Internet::start() {
  return Internet::type == Internet::WIFI ? WiFiConn::start() : GsmConn::start();
}

void Internet::stop() {
  Internet::type == Internet::WIFI ? WiFiConn::stop() : GsmConn::stop();
}

Http::Response Internet::httpGet(String& url, String authorization) {
  return Internet::type == Internet::WIFI ? WiFiConn::httpGet(url, authorization)
                                          : GsmConn::httpGet(url, authorization);
}

Http::Response Internet::httpPost(const String& url, String& body) {
  return Internet::httpPost(url, body);
}

Http::Response Internet::httpPost(String& url, String& body) {
  String authorization = String("Bearer ") + Config::getAccessToken();
  return Internet::httpPost(url, body, authorization);
}

Http::Response Internet::httpPost(String& url, String& body, String& authorization) {
  return Internet::type == Internet::WIFI ? WiFiConn::httpPost(url, body, authorization)
                                          : GsmConn::httpPost(url, body, authorization);
}

Http::Response Internet::httpPut(String& url, String& body, String authorization) {
  return Internet::type == Internet::WIFI ? WiFiConn::httpPut(url, body, authorization)
                                          : GsmConn::httpPut(url, body, authorization);
}

bool Internet::isConnected() {
  return Internet::type == Internet::WIFI ? WiFiConn::isConnected() : GsmConn::isConnected();
}

void Internet::setType(Type type) {
  Internet::type = type;
}
