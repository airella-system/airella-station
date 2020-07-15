#include <Arduino.h>
#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/AirSensor.h"
#include "device/PowerSensor.h"
#include "device/WeatherSensor.h"
#include "maintenance/Logger.h"

#include "core/Core.h";

Core core;

bool refreshRequested = false;

class MyBluetoothHandler : public BluetoothHandler {
  void deviceRefreshRequest() {
    Serial.println("Refresh request!");
    refreshRequested = true;
  }
};

void setup() {
  core.setUp();
  Serial.begin(115200);
  // Config::load();
  // Bluetooth::start(new MyBluetoothHandler());
  // Internet::setType(Internet::WIFI);
  // Internet::start();
  // Internet::httpGet("...");
  // AirSensor::init();
  // AirSensor::powerOn();
  // WeatherSensor::init();
  PowerSensor::init();
}

void loop() {
  core.loop();
  // if (refreshRequested) {
  //   Logger::debug("conf");
  //   Config::save();
  //   Api.configUpdated();
  //   refreshRequested = false;
  // }
  Logger::debug("work");

  // String tmp = String("Temperature ") + WeatherSensor::getTemperature();
  // Logger::debug(&tmp);

  // tmp = String("Pressure ") + WeatherSensor::getPressure();
  // Logger::debug(&tmp);

  // tmp = String("Humidity ") + WeatherSensor::getHumidity();
  // Logger::debug(&tmp);

  String tmp = String("Power: ") + PowerSensor::getPower();
  Logger::debug(&tmp);
  tmp = String("Voltage: ") + PowerSensor::getBusVoltage();
  Logger::debug(&tmp);

  tmp = String("Current: ") + PowerSensor::getCurrent();
  Logger::debug(&tmp);

  delay(1000);
}