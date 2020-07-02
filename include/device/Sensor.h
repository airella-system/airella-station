#pragma once

#include "maintenance/Logger.h"
#include "config/HardwareConfig.h"
#include <Arduino.h>

class Sensor {
protected:
    bool isInit();
    bool initialized = false;
};