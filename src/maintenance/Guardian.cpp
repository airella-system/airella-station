#include "maintenance/Guardian.h"

bool Guardian::isDeviceOk() {
  //todo: it will be implemented with troubleshooting and error control.
  return true;
}

void Guardian::checkWiFiConnection() {
  if(WiFi.status() != WL_CONNECTED) {
    reconectWiFi();
  }
}

bool Guardian::reconectWiFi() {
  int tryCount = 5;
  while(WiFi.status() != WL_CONNECTED && tryCount > 0) {
    tryCount--;
    WiFi.reconnect();
    delay(1000);
  }
  if(WiFi.status() == WL_CONNECTED){
    Logger::info("[Guardian::reconectWiFi()] Reconnect to WiFi");
    return true;
  }

  Logger::info("[Guardian::reconectWiFi()] Unable to reconnect WiFi connection");
  return false;
}
