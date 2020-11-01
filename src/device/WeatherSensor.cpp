#include "device/WeatherSensor.h"

WeatherSensor::WeatherSensor() : i2cBus(config.i2cBusNum) {
  Logger::info("[WeatherSensor] Initalizing ...");
  setTextState("WEATHER_SENSOR|INIT");

  if (i2cBus.begin(config.dataLinePin, config.clockLinePin, 100000)) {
    Logger::info("[WeatherSensor] Initialized I2C bus: OK.");
  } else {
    Logger::error("[WeatherSensor] Unable to initialize I2C bus.");
    setTextState("WEATHER_SENSOR|INIT_ERROR");
    return;
  }

  if (bmeDevice.begin(config.sensorAddress, &i2cBus)) {
    Logger::info("[WeatherSensor] Initialized BME device: OK.");
  } else {
    Logger::error("[WeatherSensor] Unable to find BME device.");
    setTextState("WEATHER_SENSOR|INIT_ERROR");
    return;
  }

  initialized = true;
  Logger::info("[WeatherSensor] Initalized is OK.");
  setTextState("WEATHER_SENSOR|OK");
}

WeatherSensor::~WeatherSensor() {}

float WeatherSensor::getTemperature() {
  return bmeDevice.readTemperature();
}

float WeatherSensor::getPressure() {
  return bmeDevice.readPressure();
}

float WeatherSensor::getHumidity() {
  return bmeDevice.readHumidity();
}