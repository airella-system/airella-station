#pragma once

#include <Arduino.h>
#include "maintenance/Logger.h"
#include "config/HardwareConfig.h"
#include <Adafruit_INA219.h>
#include <Wire.h>

struct PowerInfo {
    float shounVoltage;
    float busVoltage;
    float current;
    float power;
    float loadVoltage;
};

class PowerSensor {

public:
    PowerSensor();
    float getShounVoltage();
    float getBusVoltage();
    float getCurrent();
    float getPower();
    float getLoadVoltage();
    PowerInfo getPowerInfo();

private:
    PowerSensorConfig config;
    Adafruit_INA219 inaDevice;
    TwoWire i2cBus;

};