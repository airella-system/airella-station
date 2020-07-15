#pragma once

#include <Arduino.h>
#include "config/HardwareConfig.h"
#include "maintenance/Logger.h"

class Sensor {
 protected:
  bool isInit();
  bool initialized = false;
};