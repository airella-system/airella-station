#pragma once

struct WeatherSensorConfig {
  const unsigned char i2cBusNum = 1;
  const unsigned char sensorAddress = 0x76;
  const unsigned char dataLinePin = 21;
  const unsigned char clockLinePin = 22;
};

class AirAndGpsSensorStrategyConfig {
 public:
  static const unsigned char uartNum = 1;
  static const unsigned char uartRx = 2;
  static const unsigned char uartTx = 13;
  static const unsigned char airSensorOrGpsMultiplexerPin = 12;
  static const unsigned char airSensorMultiplexerLogicValue = 0;
  static const unsigned char gpsMultiplexerLogicValue = 1;
};

class AirSensorConfig {
 public:
  static const unsigned char powerPin = 26;
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
