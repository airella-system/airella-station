#include "maintenance/Guardian.h"

unsigned long Guardian::lastStatisticTimestamp = 0;
DataPersister* Guardian::measurePersister = nullptr;
DataPersister* Guardian::logPersister = nullptr;

String Guardian::getDeviceState() {
  String state = "";
  state += "REGISTERED|";
  state += Api.isRegistered() ? "YES" : "NO";

  #ifdef IS_MOCK
  state += ",";
  state += "API_CONNECTION|";
  state += "OK";
  state += ",";
  state += "AIR_SENSOR|";
  state += "ERROR";
  state += ",";
  state += "GPS|";
  state += "OK";
  state += ",";
  state += "HEATER|";
  state += "ERROR";
  state += ",";
  state += "POWER_SENSOR|";
  state += "OK";
  state += ",";
  state += "WEATHER_SENSOR|";
  state += "ERROR";
  #endif

  #ifndef IS_MOCK
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
  #endif

  return state;
}

void Guardian::checkWiFiConnection() {
  if(WiFi.status() != WL_CONNECTED) {
    if(reconectWiFi()) {
      if(!timeProvider.isInitialized()) {
        timeProvider.connect();
      }
      timeProvider.update();
      timeProvider.persistTime();
    }
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

void Guardian::check() {
  
}

void Guardian::tryFlushBuffers() {
  if(measurePersister != nullptr) measurePersister->flushBuffer(DataPersister::BufferType::measurementBuferType);
  if(logPersister != nullptr) logPersister->flushBuffer(DataPersister::BufferType::logBuffer);
}

void Guardian::clearStorage() {
  FS* storage = DeviceContainer.storage->getStorage();
  storage->rmdir("/measurements");
  storage->rmdir("/logs");
  storage->rmdir("/sbuffer");
  StorableBuffer::createCatalogStructure("m");
  storage->mkdir("/measurements");
  storage->mkdir("/logs");
  Logger::info("[Guardian::clearStorage()] Storage is clear.");
}
