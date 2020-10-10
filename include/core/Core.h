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
#include "config/MeasurementType.h"
#include "device/AirSensor.h"
#include "device/GpsSensor.h"
#include "core/AirAndGpsSensorStrategy.h"
#include "device/Heater.h"
#include "device/PowerSensor.h"
#include "device/Storage.h"
#include "device/WeatherSensor.h"
#include "maintenance/Logger.h"
#include "time/Time.h"
#include "maintenance/Statistics.h"
#include "maintenance/Guardian.h"
#include "device/DeviceContainer.h"
#include "config/HardwareConfig.h"

class Core {
public:
  Core();
  void setUp();
  void main();
  void reset();

private:
  WeatherSensor *weatherSensor = NULL;
  PowerSensor *powerSensor = NULL;
  Heater *heater = NULL;
  Storage *storage = NULL;
  GSM *gsm = NULL;
  AirAndGpsSensorStrategy *airAndGpsSensorStrategy = NULL;

  MeasurementType measurementType;
  unsigned long lastPublishMillis = 0;
  unsigned long lastGpsUpdateMillis = 0;
  bool isWorking = true;
};

extern Core core;
