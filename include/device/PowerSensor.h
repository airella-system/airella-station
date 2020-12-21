#pragma once

#include <Adafruit_INA219.h>
#include <Wire.h>
#include "device/Sensor.h"

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
  void begin();
  float getBusVoltage();
  float getCurrent();
  float getPower();
  float getLoadVoltage();
  const PowerInfo getPowerInfo();

 private:
  PowerSensorConfig config;
  Adafruit_INA219 inaDevice;
  TwoWire i2cBus;
};