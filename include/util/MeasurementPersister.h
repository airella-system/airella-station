#pragma once

#include <Arduino.h>
#include "device/DeviceContainer.h"

class MeasurementPersister {
public:
  MeasurementPersister() {
    storage = DeviceContainer.storage->getStorage();
  };
  ~MeasurementPersister() {};
  void save(String& data);
private:
  FS* storage;
};
