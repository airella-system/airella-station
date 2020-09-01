#include <Arduino.h>
#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/bluetooth/BluetoothRefreshHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

bool refreshRequested = false;
unsigned long lastPublishMillis = 0;

void setup() {
  Config::load();
  Bluetooth::start(new BluetoothRefreshHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();
}

void loop() {
  if ((millis() - lastPublishMillis) > 10000) {
    Logger::debug("Publishing data");
    double temperature = random(15 * 100, 25 * 100) / 100.0;
    double humidity = random(0 * 100, 100 * 100) / 100.0;
    double pressure = random(950 * 100, 1050 * 100) / 100.0;
    double pm1 = random(20 * 100, 40 * 100) / 100.0;
    double pm2_5 = random(10 * 100, 20 * 100) / 100.0;
    double pm10 = random(20 * 100, 40 * 100) / 100.0;

    Api.publishMeasurement("temperature", temperature);
    Api.publishMeasurement("humidity", humidity);
    Api.publishMeasurement("pressure", pressure);
    Api.publishMeasurement("pm1", pm1);
    Api.publishMeasurement("pm2_5", pm2_5);
    Api.publishMeasurement("pm10", pm10);

    lastPublishMillis = millis();
  }
  delay(100);
}