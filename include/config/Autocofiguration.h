#pragma once

#include "maintenance/Logger.h"

class Autocofiguration {
public:
  Autocofiguration() {}
  ~Autocofiguration() {}
  void tryConfig();
private:
  bool isConfigured();
  bool configFileExists();
  bool configFileIsCorect();
  void parseConfigFile(/*pointer here*/);
  void setConfig();
};
