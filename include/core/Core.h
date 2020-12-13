#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/bluetooth/Bluetooth.h"
#include "communication/bluetooth/BluetoothHandler.h"
#include "communication/bluetooth/BluetoothRefreshHandler.h"
#include "communication/common/Internet.h"
#include "communication/gsm/GSM.h"
#include "config/Config.h"
#include "config/HardwareConfig.h"
#include "config/MeasurementType.h"
#include "core/AirAndGpsSensorStrategy.h"
#include "device/AirSensor.h"
#include "device/DeviceContainer.h"
#include "device/GpsSensor.h"
#include "device/Heater.h"
#include "device/PowerSensor.h"
#include "device/Storage.h"
#include "device/WeatherSensor.h"
#include "maintenance/Guardian.h"
#include "maintenance/Logger.h"
#include "maintenance/Statistics.h"
#include "time/Time.h"
#include "Task.h"
#include "TaskHandler.h"
#include "maintenance/Guardian.h"
#include "device/DeviceContainer.h"
#include "config/HardwareConfig.h"
#include "util/StorableBuffer.h"
#include "config/Autocofiguration.h"
#include "api/entity/DataModel.h"

class Core {
public:
  Core() : storableBuffer("m") {};
  ~Core() {};
  void setUp();
  void main();
  bool isError();
  void reset();
  void doCoreTasks();
  void doMeasurements(DataModel& dataModel);
  void checkGpsLocation(DataModel& dataModel);

 private:
  WeatherSensor *weatherSensor = NULL;
  PowerSensor *powerSensor = NULL;
  Heater *heater = NULL;
  Storage *storage = NULL;
  GSM *gsm = NULL;
  AirAndGpsSensorStrategy *airAndGpsSensorStrategy = NULL;

  MeasurementType measurementType;
  unsigned long lastPublishMillis = 0;
  unsigned long lastCheckMillis = 0;
  unsigned long lastGpsUpdateMillis = 0;
  bool isWorking = true;
  TaskHandler<void*, double, String>* taskHandler;
  StorableBuffer storableBuffer;
  void tryPublishMeasurement();
};

extern Core core;
