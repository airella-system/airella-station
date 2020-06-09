#pragma once

#include "maintenance/Logger.h"
#include "config/HardwareManager.h"
#include <Arduino.h>

class Sensor {
protected:
    static bool isInit();
    static bool initialized;
};