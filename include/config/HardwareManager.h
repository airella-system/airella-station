#pragma once

// pm sensor
#define U1RX 2
#define U1TX 13
#define LED1 14
#define PMS7K_ON 26

// weather sensor
#define BME_SDA 21
#define BME_SCL 22
#define WEATHER_SENEOR_I2C_BUS_NO 1

// power sesnor
#define POWER_SENEOR_I2C_BUS_NO 0
#define INA_SDA 4
#define INA_SCL 27

struct UartConfig {
  bool isOk = false;
  unsigned char rx = -1;
  unsigned char tx = -1;

  UartConfig(){};

  UartConfig(const unsigned char _rx, const unsigned char _tx)
      : isOk(true), rx(_rx), tx(_tx){};
};

struct TwoWireConfig {
  bool isOk = false;
  unsigned char in = -1;
  unsigned char out = -1;

  TwoWireConfig(){};

  TwoWireConfig(const unsigned char _in, const unsigned char _out)
      : isOk(true), in(_in), out(_out){};
};

template <class T> struct OptionalConfig {
  bool isOk = false;
  T value = 0;

  OptionalConfig(){};

  OptionalConfig(const T _value) : isOk(true), value(_value){};
};

enum ComponentId {
  uartForAirSensor = 0,
  airSensorPowerPin = 1,
  uartForAirLed = 2,
  i2cForWeatherSensor = 3,
  i2cConfigForWeatherSensor = 4,
  i2cForPowerSensor = 5,
  i2cConfigForPowerSensor = 6
};

class HardwareManager {
public:
  static UartConfig getUartForAirSensor();
  static void releaseUartForAirSensor();

  static OptionalConfig<unsigned char> getAirSensorPowerPin();
  static void releaseAirSensorPowerPin();

  static OptionalConfig<unsigned char> getAirSensorLed();
  static void releaseAirSensorLed();

  static OptionalConfig<unsigned char> getBusNumForWeatherSensor();
  static void releaseBusNumForWeatherSensor();

  static TwoWireConfig getI2CConfigForWeatherSensor();
  static void releaseI2CConfigForWeatherSensor();

  static OptionalConfig<unsigned char> getBusNumForPowerSensor();
  static void releaseBusNumForPowerSensor();

  static TwoWireConfig getI2CConfigForPowerSensor();
  static void releaseI2CConfigForPowerSensor();

private:
  static bool componentState[7];
  static OptionalConfig<unsigned char> getPin(unsigned char pin, bool *pinStatePtr, const char *message);
  static void releasePin(bool *pinStatePtr, const char *message);
  static void setComponentState(ComponentId componentId, bool value);
  static bool getComponentState(ComponentId componentId);
  static bool *getComponentStatePtr(ComponentId componentId);
};