#pragma once

#include "config/Config.h"
#include "maintenance/Logger.h"
#include "device/DeviceContainer.h"
#include <ArduinoJson.h>

class Autocofiguration {
public:
  Autocofiguration() {}
  ~Autocofiguration() {}
  void tryConfig();
private:
  const char* configFileName = "/airella.conf";
  bool isConfigured();
  bool configFileExists();
  bool parseConfigFile(DynamicJsonDocument* configDoc);
  bool configFileIsCorect(DynamicJsonDocument* configDoc);
  void setConfig(DynamicJsonDocument* configDoc);
};
