#include "maintenance/Guardian.h"

unsigned long Guardian::lastStatisticTimestamp = 0;

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

void Guardian::statistics() {
  // if (abs(millis() - lastStatisticTimestamp) > 1000 * 60 * 10) {
  if (abs(millis() - lastStatisticTimestamp) > 1000 * 60 * 0.5) { // just for debug
      Logger::info("[Guardian::statistics]: Sending stats");

      Statistics.reportHeartbeat();
      Statistics.reportConnectionState();
      Statistics.reportPower();
      Statistics.reportPm();
      Statistics.reportWeather();

      lastStatisticTimestamp = millis();
    }
}
