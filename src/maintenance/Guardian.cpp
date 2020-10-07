#include "maintenance/Guardian.h"

unsigned long Guardian::lastStatisticTimestamp = 0;

String Guardian::getDeviceState() {
  String state = "";
  state += "REGISTERED|";
  state += Api.isRegistered() ? "YES" : "NO";
  state += ",";
  state += "API_CONNECTION|";
  state += core.isError() ? "ERROR" : "OK";
  state += ",";
  state += DeviceContainer.airSensor->getTextState();
  state += ",";
  state += DeviceContainer.gpsSensor->getTextState();
  state += ",";
  state += DeviceContainer.heater->getTextState();
  state += ",";
  state += DeviceContainer.powerSensor->getTextState();
  state += ",";
  state += DeviceContainer.weatherSensor->getTextState();
  return state;
}

void Guardian::checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    reconectWiFi();
  }
}

bool Guardian::reconectWiFi() {
  int tryCount = 5;
  while (WiFi.status() != WL_CONNECTED && tryCount > 0) {
    tryCount--;
    WiFi.reconnect();
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Logger::info("[Guardian::reconectWiFi()] Reconnect to WiFi");
    return true;
  }

  Logger::info("[Guardian::reconectWiFi()] Unable to reconnect WiFi connection");
  return false;
}

void Guardian::statistics() {
  // if (abs(millis() - lastStatisticTimestamp) > 1000 * 60 * 10) {
  if (abs(millis() - lastStatisticTimestamp) > 1000 * 60 * 0.5) {  // just for debug
    Logger::info("[Guardian::statistics]: Sending stats");

    Statistics.reportHeartbeat();
    Statistics.reportConnectionState();
    Statistics.reportConnectionType();
    Statistics.reportPower();
    Statistics.reportHeater();

    lastStatisticTimestamp = millis();
  }
}
