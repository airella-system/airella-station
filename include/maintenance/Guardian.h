#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "device/DeviceContainer.h"
#include "api/Api.h"
#include "core/Core.h"

class Guardian {
public:
  static String getDeviceState();
  static void checkWiFiConnection();
  static bool reconectWiFi();
  static void statistics();
private:
  static unsigned long lastStatisticTimestamp;
};