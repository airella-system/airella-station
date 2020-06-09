#pragma once

#include "device/Sensor.h"
#include <Adafruit_BME280.h>
#include <Wire.h>

// try 0x77 or 0x76
#define SENSOR_ADDR 0x76

class WeatherSensor : public Sensor {
public:
  static void init();
  static float getTemperature();
  static float getPressure();
  static float getHumidity();

private:
  static Adafruit_BME280 bmeDevice;
  static TwoWire i2cBus;
};