#include "config/HardwareManager.h"
#include "maintenance/Logger.h"

void HardwareManager::setComponentState(const ComponentId componentId, const bool value) {
  componentState[componentId] = value;
}

bool HardwareManager::getComponentState(const ComponentId componentId) {
  return componentState[componentId];
}

bool *HardwareManager::getComponentStatePtr(const ComponentId componentId) {
  return &componentState[componentId];
}

UartConfig HardwareManager::getUartForAirSensor() {
  if (uartForAirSensor) {
    Logger::error("[HardwareManager::getUartForAirSensor] already taken.");
    return UartConfig();
  }

  setComponentState(uartForAirSensor, true);
  return UartConfig(U1RX, U1TX);
};

void HardwareManager::releaseUartForAirSensor() {
  if (!uartForAirSensor) {
    Logger::warning(
        "[HardwareManager::releaseUartForAirSensor] already released.");
    return;
  }
  setComponentState(uartForAirSensor, false);
}

OptionalConfig<unsigned char> HardwareManager::getAirSensorPowerPin() {
  return getPin(PMS7K_ON, getComponentStatePtr(uartForAirSensor),
                "[HardwareManager::getAirSensorPowerPin] already taken.");
};

void HardwareManager::releaseAirSensorPowerPin() {
  releasePin(getComponentStatePtr(airSensorPowerPin),
             "[HardwareManager::getAirSensorPowerPin] already released.");
}

OptionalConfig<unsigned char> HardwareManager::getAirSensorLed() {
  return getPin(LED1, getComponentStatePtr(uartForAirLed),
                "[HardwareManager::getAirSensorLed] already taken.");
}

void HardwareManager::releaseAirSensorLed() {
  releasePin(getComponentStatePtr(uartForAirLed),
             "[HardwareManager::releaseAirSensorLed] already released.");
}

OptionalConfig<unsigned char> HardwareManager::getBusNumForWeatherSensor() {
  return getPin(WEATHER_SENEOR_I2C_BUS_NO, getComponentStatePtr(i2cForWeatherSensor),
                "[HardwareManager::getBusNumForWeatherSensor] already taken.");
}

void HardwareManager::releaseBusNumForWeatherSensor() {
  releasePin(getComponentStatePtr(i2cForWeatherSensor),
             "[HardwareManager::getBusNumForWeatherSensor] already released.");
}

TwoWireConfig HardwareManager::getI2CConfigForWeatherSensor() {
  if (getComponentState(i2cConfigForWeatherSensor)) {
    Logger::error("[HardwareManager::getI2CConfigForWeatherSensor] already taken.");
    return TwoWireConfig();
  }

  setComponentState(i2cConfigForWeatherSensor, true);
  return TwoWireConfig(BME_SDA, BME_SCL);
}

void HardwareManager::releaseI2CConfigForWeatherSensor() {
  if (!getComponentState(i2cConfigForWeatherSensor)) {
    Logger::warning(
        "[HardwareManager::releaseI2CConfigForWeatherSensor] already released.");
    return;
  }
  setComponentState(i2cConfigForWeatherSensor, false);
}

OptionalConfig<unsigned char> HardwareManager::getBusNumForPowerSensor() {
  return getPin(POWER_SENEOR_I2C_BUS_NO, getComponentStatePtr(i2cForPowerSensor),
                "[HardwareManager::getBusNumForWeatherSensor] already taken.");
}

void HardwareManager::releaseBusNumForPowerSensor() {
  releasePin(getComponentStatePtr(i2cForPowerSensor),
             "[HardwareManager::getBusNumForWeatherSensor] already released.");
}

TwoWireConfig HardwareManager::getI2CConfigForPowerSensor() {
  if (getComponentState(i2cConfigForPowerSensor)) {
    Logger::error("[HardwareManager::getI2CConfigForPowerSensor] already taken.");
    return TwoWireConfig();
  }

  setComponentState(i2cConfigForPowerSensor, true);
  return TwoWireConfig(INA_SDA, INA_SCL);
}

void HardwareManager::releaseI2CConfigForPowerSensor() {
  if (!getComponentState(i2cConfigForPowerSensor)) {
    Logger::warning(
        "[HardwareManager::releaseI2CConfigForPowerSensor] already released.");
    return;
  }
  setComponentState(i2cConfigForPowerSensor, false);
}

OptionalConfig<unsigned char>
HardwareManager::getPin(unsigned char pin, bool *pinPtr, const char *message) {
  if (pinPtr) {
    Logger::error(message);
    return OptionalConfig<unsigned char>();
  }
  return OptionalConfig<unsigned char>(pin);
}

void HardwareManager::releasePin(bool *pinPtr, const char *message) {
  if (!pinPtr) {
    Logger::warning(message);
    return;
  }
  *pinPtr = false;
}
