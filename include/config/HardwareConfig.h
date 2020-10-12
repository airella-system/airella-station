#pragma once

struct WeatherSensorConfig {
  const unsigned char i2cBusNum = 1;
  const unsigned char sensorAddress = 0x76;
  const unsigned char dataLinePin = 21;
  const unsigned char clockLinePin = 22;
};

struct AirAndGpsSensorStrategyConfig {
  const unsigned char uartNum = 1;
  const unsigned char uartRx = 2;
  const unsigned char uartTx = 13;
  const unsigned char airSensorOrGpsMultiplexerPin = 12;
  const unsigned char airSensorMultiplexerLogicValue = 0;
  const unsigned char gpsMultiplexerLogicValue = 1;
};

struct AirSensorConfig {
  const unsigned char powerPin = 26;
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

struct GSMConfig {
  const unsigned char serialLine = 2;
  const unsigned char powerPin = 33;
  const unsigned char uartRx = 16;
  const unsigned char uartTx = 17;
};
