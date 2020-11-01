#pragma once

#include <Arduino.h>
#include <FS.h>
#include "time/Time.h"

class MeasurementPersister {
public:
  MeasurementPersister();
  ~MeasurementPersister() {};
  void save(String& sensor, String data);
private:
  FS* storage;
};
