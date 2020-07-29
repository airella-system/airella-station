#pragma once

#include <Arduino.h>
#include "config/HardwareConfig.h"
#include "maintenance/Logger.h"

class Sensor {
  public:
    bool isInit();
  protected:
    bool initialized = false;
};