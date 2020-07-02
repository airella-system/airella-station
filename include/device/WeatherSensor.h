#pragma once

#include "device/Sensor.h"
#include <Adafruit_BME280.h>
#include <Wire.h>

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