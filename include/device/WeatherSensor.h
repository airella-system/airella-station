#pragma once

#include <Adafruit_BME280.h>
#include <Wire.h>
#include "device/Sensor.h"

class WeatherSensor : public Sensor {
 public:
  WeatherSensor();
  ~WeatherSensor();
  float getTemperature();
  float getPressure();
  float getHumidity();

 private:
  WeatherSensorConfig config;
  Adafruit_BME280 bmeDevice;
  TwoWire i2cBus;
};