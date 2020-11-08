#pragma once

#include <Arduino.h>
#include <FS.h>
#include "time/Time.h"

class DataPersister {
public:
  DataPersister();
  ~DataPersister() {};
  void saveMeasurement(String& sensor, String data);
  void saveLog(String& logMessage);
private:
  FS* storage;
  bool initialized = false;
};
