#pragma once

#include "config/Config.h"
#include "device/AirSensor.h"
#include "device/GpsSensor.h"
#include "device/Heater.h"
#include "device/PowerSensor.h"
#include "device/Storage.h"
#include "util/StorableBuffer.h"
#include "util/DataPersister.h"
#include "device/WeatherSensor.h"

class Heater;

class DeviceContainerClass {
 public:
  friend class Core;
  friend class Guardian;
  friend class StatisticsClass;
  friend class StorableBuffer;
  friend class DataPersister;
  
private:
  AirSensor *airSensor = NULL;
  WeatherSensor *weatherSensor = NULL;
  PowerSensor *powerSensor = NULL;
  Heater *heater = NULL;
  Storage *storage = new Storage();
  GpsSensor *gpsSensor = NULL;
};

extern DeviceContainerClass DeviceContainer;
