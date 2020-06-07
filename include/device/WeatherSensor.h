#pragma once

#include <Adafruit_BME280.h>
#include <Arduino.h>
#include <Wire.h>

// try 0x77 or 0x76
#define SENSOR_ADDR 0x76

class WeatherSensor {
public:
  static void init();
  static bool isInit();
  static float getTemperature();
  static float getPreassure();
  static float getHumidity();

private:
  static bool initialized;
  static Adafruit_BME280 bmeDevice;
  static TwoWire i2cBus;
};