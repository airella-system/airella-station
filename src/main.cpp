#include <Arduino.h>
#include "Config.h"
#include "Bluetooth.h"
#include "BluetoothHandler.h"

#include "Internet.h"

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
}

void loop()
{
  delay(2000);
}
