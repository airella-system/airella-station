#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "config/Config.h"
#include "maintenance/Logger.h"

class Guardian {
public:
  static bool isDeviceOk();
  static void checkWiFiConnection();
  static bool reconectWiFi();
};