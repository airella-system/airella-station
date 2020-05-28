#include <Arduino.h>
#include <Config.h>
#include <Bluetooth.h>

void setup()
{
  Serial.begin(115200);
  Config::instance().load();
  Bluetooth::start();
}

void loop()
{
  delay(2000);
}
