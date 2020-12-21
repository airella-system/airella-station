#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "api/Api.h"
#include "config/Config.h"
#include "core/Core.h"
#include "device/DeviceContainer.h"
#include "maintenance/Logger.h"
#include "api/entity/DataModel.h"
#include "maintenance/StatisticTypes.h"

class Guardian {
public:
  static String getDeviceState();
  static void checkWiFiConnection();
  static bool reconectWiFi();
  static void statistics(DataModel& dataModel);
  static void check();
  static void tryFlushBuffers();
  static void clearStorage();
  static DataPersister* measurePersister;
  static DataPersister* logPersister;

private:
  static unsigned long lastStatisticTimestamp;
};