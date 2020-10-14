#pragma once

#include "config/Config.h"
#include "maintenance/Logger.h"
#include "device/AirSensor.h"
#include "device/WeatherSensor.h"
#include "device/Heater.h"
#include "device/PowerSensor.h"
#include "device/Storage.h"
#include "util/StorableBuffer.h"
#include "util/MeasurementPersister.h"

class Heater;

class DeviceContainerClass {
public:
  friend class Core;
  friend class Guardian;
  friend class StatisticsClass;
  friend class StorableBuffer;
  friend class MeasurementPersister;
private:
  AirSensor *airSensor = NULL;
  WeatherSensor *weatherSensor = NULL;
  PowerSensor *powerSensor = NULL;
  Heater *heater = NULL;
  Storage *storage = NULL;
};

extern DeviceContainerClass DeviceContainer;
