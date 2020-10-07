#pragma once

#include <Arduino.h>
#include "config/HardwareConfig.h"
#include "maintenance/Logger.h"

class Sensor {
  public:
    bool isInit();
    void setTextState(String string);
    String getTextState();

  protected:
    bool initialized = false;
    String stringState = "stringState";
};