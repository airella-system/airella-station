#pragma once

struct WeatherSensorConfig {
  const unsigned char i2cBusNum = 1;
  const unsigned char sensorAddress = 0x76;
  const unsigned char dataLinePin = 21;
  const unsigned char clockLinePin = 22;
};

class AirSensorConfig {
public:
  static const unsigned char serialNumber = 1;
  static const unsigned char powerPin = 26;
  static const unsigned char uartRx = 2;
  static const unsigned char uartTx = 13;
};

struct PowerSensorConfig {
  const unsigned char i2cBusNum = 0;
  const unsigned char sensorAddress = 0x40;
  const unsigned char dataLinePin = 27;
  const unsigned char clockLinePin = 4;
};

struct HeaterConfig {
  const unsigned char oneWirePin = 15;
  const unsigned char analogPin = 25;
};

struct StorageConfig {
  const unsigned char sdPin = 5;
};
