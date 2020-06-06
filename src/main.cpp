#include <Arduino.h>
#include "config/Config.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/common/Internet.h"
#include "maintaince/Logger.h"
#include "device/AirSensor.h"

class MyBluetoothHandler : public BluetoothHandler
{
    void deviceRefreshRequest()
    {
        Serial.println("Refresh request!");
        Config::instance().save();
        Internet::start();
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
  Logger::info("test");
  delay(2000);
}
