#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include <Arduino.h>
#include <ArduinoJson.h>

bool refreshRequested = false;
unsigned long lastPublishMillis = 0;

class MyBluetoothHandler : public BluetoothHandler {
  void deviceRefreshRequest() {
    Serial.println("Refresh request!");
    refreshRequested = true;
  }
};

void setup() {
  Serial.begin(115200);
  Config::load();
  Config::save();
  Bluetooth::start(new MyBluetoothHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();
}

void loop() {
  if (refreshRequested) {
    Config::save();
    bool wasRegistered = Api.isRegistered();
    Api.configUpdated();
    if (!wasRegistered) {
        double latitudeMin = 49.972368;
        double latitudeMax = 50.137422;
        double longitudeMin = 20.435403;
        double longitudeMax = 20.735403;

        double latitude = random(latitudeMin * 1000000, latitudeMax * 1000000) / 1000000.0;
        double longitude = random(longitudeMin * 1000000, longitudeMax * 1000000) / 1000000.0;

        Api.publishLocation(latitude, longitude);
    }
    refreshRequested = false;
  }

  if ((millis() - lastPublishMillis) > 10000) {
    Logger::debug("Publishing data");
    double temperature = random(15 * 100, 25 * 100) / 100.0;
    double humidity = random(0 * 100, 100 * 100) / 100.0;
    double pm1 = random(20 * 100, 40 * 100) / 100.0;
    double pm2_5 = random(10 * 100, 20 * 100) / 100.0;
    double pm10 = random(20 * 100, 40 * 100) / 100.0;

    Api.publishMeasurement("temperature", temperature);
    Api.publishMeasurement("humidity", humidity);
    Api.publishMeasurement("pm1", pm1);
    Api.publishMeasurement("pm2_5", pm2_5);
    Api.publishMeasurement("pm10", pm10);

    lastPublishMillis = millis();
  }
  delay(100);
}