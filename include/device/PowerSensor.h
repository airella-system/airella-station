#pragma once

#include "device/Sensor.h"
#include <Adafruit_INA219.h>
#include <Wire.h>

//TODO: move to save place in hardware manager
#define INA_SDA 4
#define INA_SCL 27

struct PowerInfo {
    float shounVoltage;
    float busVoltage;
    float current;
    float power;
    float loadVoltage;
};

class PowerSensor : public Sensor {
public:
    static void init();
    static float getShounVoltage();
    static float getBusVoltage();
    static float getCurrent();
    static float getPower();
    static float getLoadVoltage();
    static PowerInfo getPowerInfo();
private:
    static Adafruit_INA219 inaDevice;
    static TwoWire i2cBus;
};