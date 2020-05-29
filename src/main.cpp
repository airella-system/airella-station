#include <Arduino.h>
#include "Config.h"
#include "Bluetooth.h"
#include "Internet.h"

void setup()
{
  Serial.begin(115200);
  Config::instance().load();
  Bluetooth::start();
  Internet::setType(Internet::WIFI);
  // Internet::httpGet("...");
}

void loop()
{
  delay(2000);
}
