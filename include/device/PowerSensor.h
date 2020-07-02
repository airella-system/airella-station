#pragma once

#include "device/Sensor.h"
#include <Adafruit_INA219.h>
#include <Wire.h>

struct PowerInfo {
    float shounVoltage;
    float busVoltage;
    float current;
    float power;
    float loadVoltage;
};

class PowerSensor : public Sensor {

public:
    PowerSensor();
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