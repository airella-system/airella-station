#include "device/PowerSensor.h"

PowerSensor::PowerSensor() : inaDevice(config.sensorAddress), i2cBus(config.i2cBusNum) {}

void PowerSensor::begin() {
  setTextState("POWER_SENSOR|INIT");
  Logger::info("[PowerSensor] Initalizing ...");

  if (i2cBus.begin(config.clockLinePin, config.dataLinePin, 100000)) {
    Logger::info("[PowerSensor] Initialized I2C bus: OK.");
  } else {
    Logger::error("[PowerSensor] Unable to initialize I2C bus.");
    setTextState("POWER_SENSOR|INIT_ERROR");
    return;
  }

  if (inaDevice.begin(&i2cBus)) {
    Logger::info("[PowerSensor] Initialized INA device: OK.");
  } else {
    Logger::error("[PowerSensor] Unable to find INA device.");
    setTextState("POWER_SENSOR|INIT_ERROR");
    return;
  }

  initialized = true;
  Logger::info("[PowerSensor] Initalized is OK.");
  setTextState("POWER_SENSOR|OK");
}

float PowerSensor::getBusVoltage() {
  return inaDevice.getBusVoltage_V();
}

float PowerSensor::getCurrent() {
  return inaDevice.getCurrent_mA();
}

float PowerSensor::getPower() {
  return inaDevice.getPower_mW();
}

float PowerSensor::getLoadVoltage() {
  float shuntvoltage = inaDevice.getShuntVoltage_mV();
  float busvoltage = inaDevice.getBusVoltage_V();
  return busvoltage + (shuntvoltage / 1000);
}

const PowerInfo PowerSensor::getPowerInfo() {
  PowerInfo powerInfo;

  powerInfo.shounVoltage = inaDevice.getShuntVoltage_mV();
  powerInfo.busVoltage = inaDevice.getBusVoltage_V();
  powerInfo.current = inaDevice.getCurrent_mA();
  powerInfo.power = inaDevice.getPower_mW();
  powerInfo.loadVoltage = powerInfo.busVoltage + (powerInfo.shounVoltage / 1000);

  return powerInfo;
}