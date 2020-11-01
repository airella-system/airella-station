#pragma once

#include <Arduino.h>
#include "config/HardwareConfig.h"
#include "device/AirSensor.h"
#include "device/GpsSensor.h"

class AirAndGpsSensorStrategy {
 public:
  AirAndGpsSensorStrategy();
  void switchToGpsSensor();
  void switchToAirSensor();
  AirSensor* getAirSensor();
  GpsSensor* getGpsSensor();

 private:
  HardwareSerial* serial;
  AirSensor* airSensor;
  GpsSensor* gpsSensor;
  AirAndGpsSensorStrategyConfig config;
};