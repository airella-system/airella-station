#include "device/WeatherSensor.h"

Adafruit_BME280 WeatherSensor::bmeDevice = Adafruit_BME280();
TwoWire WeatherSensor::i2cBus = 0;

void WeatherSensor::init() {
  if (initialized) {
    Logger::warning("[WeatherSensor] Sensor is already initialized.");
  }

  OptionalConfig<unsigned char> pinConfig =
      HardwareManager::getBusNumForWeatherSensor();
  if (!pinConfig.isOk) {
    Logger::error("[WeatherSensor] Unable to get pin configuration.");
  }
  i2cBus = TwoWire(pinConfig.value);

  Logger::info("[WeatherSensor] Initalizing sensors.");

  if (!i2cBus.begin(BME_SDA, BME_SCL, 100000)) {
    Logger::error(
        "[WeatherSensor] Unable to initialize sensor bus communication.");
    return;
  }

  if (!bmeDevice.begin(SENSOR_ADDR, &i2cBus)) {
    Logger::error("[WeatherSensor] Unable to find sensor.");
    return;
  }

  initialized = true;
  Logger::info("[WeatherSensor] Initalizing is OK.");
}

float WeatherSensor::getTemperature() {
  if (!initialized) {
    Logger::error("[WeatherSensor] Unable to get temperature value - sensor is "
                  "uninitialized.");
  }
  return bmeDevice.readTemperature();
}

float WeatherSensor::getPressure() {
  if (!initialized) {
    Logger::error("[WeatherSensor] Unable to get pressure value - sensor is "
                  "uninitialized.");
  }
  return bmeDevice.readPressure();
}

float WeatherSensor::getHumidity() {
  if (!initialized) {
    Logger::error("[WeatherSensor] Unable to get humidity value - sensor is "
                  "uninitialized.");
  }
  return bmeDevice.readHumidity();
}