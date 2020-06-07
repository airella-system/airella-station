#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/AirSensor.h"
#include "device/WeatherSensor.h"
#include "maintenance/Logger.h"
#include <Arduino.h>
#include <ArduinoJson.h>

bool refreshRequested = false;

class MyBluetoothHandler : public BluetoothHandler {
  void deviceRefreshRequest() {
    Serial.println("Refresh request!");
    refreshRequested = true;
  }
};

void setup() {
  Serial.begin(115200);
  // Config::instance().load();
  // Bluetooth::start(new MyBluetoothHandler());
  // Internet::setType(Internet::WIFI);
  // Internet::start();
  // Internet::httpGet("...");
  // AirSensor::init();
  // AirSensor::powerOn();
  WeatherSensor::init();
}

void loop() {
  // if (refreshRequested) {
  //   Config::instance().save();
  //   Api.configUpdated();
  //   refreshRequested = false;
  // }
  Logger::debug("TEST");

  String tmp = String("Temperature ") + WeatherSensor::getTemperature();
  Logger::debug(&tmp);

  tmp = String("Pressure ") + WeatherSensor::getPressure();
  Logger::debug(&tmp);

  tmp = String("Humidity ") + WeatherSensor::getHumidity();
  Logger::debug(&tmp);

  delay(5000);
}