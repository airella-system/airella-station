#include "device/PowerSensor.h"

//TODO: move params to hardware manager
Adafruit_INA219 PowerSensor::inaDevice(0x40);
TwoWire PowerSensor::i2cBus = TwoWire(0);

void PowerSensor::init() {
    if (initialized) {
        Logger::warning("[PowerSensor] Sensor is already initialized.");
        return;
    }

    OptionalConfig<unsigned char> pinConfig = HardwareManager::getBusNumForWeatherSensor();
    if (!pinConfig.isOk) {
        Logger::error("[PowerSensor] Unable to get pin configuration.");
        return;
    }
    i2cBus = TwoWire(pinConfig.value);

    Logger::info("[PowerSensor] Initalizing sensors.");

    TwoWireConfig busConfig = HardwareManager::getI2CConfigForPowerSensor();
    if (!busConfig.isOk) {
        Logger::error("[PowerSensor] Unable to get bus configuration.");
        return;
    }

    if (!i2cBus.begin(busConfig.in, busConfig.out, 100000)) {
        Logger::error("[PowerSensor] Unable to initialize sensor bus communication.");
        return;
    }

    if (!inaDevice.begin(&i2cBus)) {
        Logger::error("[PowerSensor] Unable to find sensor.");
        return;
    }

    initialized = true;
    Logger::info("[PowerSensor] Initalized is OK.");
}

float PowerSensor::getShounVoltage() {
    return inaDevice.getShuntVoltage_mV();
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

PowerInfo PowerSensor::getPowerInfo() {
    PowerInfo powerInfo;

    powerInfo.shounVoltage = inaDevice.getShuntVoltage_mV();
    powerInfo.busVoltage = inaDevice.getBusVoltage_V();
    powerInfo.current = inaDevice.getCurrent_mA();
    powerInfo.power = inaDevice.getPower_mW();
    powerInfo.loadVoltage = powerInfo.busVoltage + (powerInfo.shounVoltage / 1000);

    return powerInfo;
}