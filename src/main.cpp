#include <Arduino.h>
#include <ArduinoJson.h>
#include "config/Config.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "maintenance/Logger.h"
#include "api/Api.h"
#include "device/AirSensor.h"

bool refreshRequested = false;
unsigned long lastPublishMillis = 0;

class MyBluetoothHandler : public BluetoothHandler
{
  void deviceRefreshRequest()
  {
    Serial.println("Refresh request!");
    refreshRequested = true;
  }
};

void setup()
{
  Serial.begin(115200);
  Config::instance().load();
  Bluetooth::start(new MyBluetoothHandler());
  Internet::setType(Internet::WIFI);
  Internet::start();
  // Internet::httpGet("...");
  // AirSensor::init();
  // AirSensor::powerOn();
}

void loop()
{
  if (refreshRequested)
  {
    Config::instance().save();
    Api.configUpdated();
    refreshRequested = false;
  }
  
  if ((millis() - lastPublishMillis) > 10000)
  {
    Logger::debug("Publishing data");
    double temperature = random(15 * 100, 25 * 100) / 100;
    double humidity = random(0 * 100, 100 * 100) / 100;
    double pm10 = random(20 * 100, 40 * 100) / 100;
    double pm25 = random(10 * 100, 20 * 100) / 100;

    Api.publishMeasurement("temperature", temperature);
    Api.publishMeasurement("humidity", humidity);
    Api.publishMeasurement("pm10", pm10);
    Api.publishMeasurement("pm25", pm25);
    lastPublishMillis = millis();
  }
  delay(100);
}
