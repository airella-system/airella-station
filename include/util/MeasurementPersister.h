#pragma once

#include <Arduino.h>
#include "device/DeviceContainer.h"
#include "time/Time.h"

class MeasurementPersister {
public:
  MeasurementPersister();
  ~MeasurementPersister() {};
  void save(String& data);
private:
  FS* storage;
};
