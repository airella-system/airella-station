#pragma once

#include <DallasTemperature.h>
#include <analogWrite.h>
#include "device/Sensor.h"

#define U1TX 13
#define OW1 15
#define HEATER_ON 25
#define ONE_WIRE_MAX_DEV 2

struct HeaterStatus {
  bool heaterIsOn;
  bool threadIsRunning;
};

class Heater : public Sensor {
public:
  Heater();
  ~Heater();
  void on();
  void off();
  void run();
  void stop();
  float getTemperature();
  HeaterStatus getHeaterState() const;
  HeaterStatus heaterStatus;

private:
  HeaterConfig config;
  float temperatureDevice[ONE_WIRE_MAX_DEV];
  DeviceAddress deviceAddress[ONE_WIRE_MAX_DEV];
  int numberOfDevices;
  OneWire communicationBus;
  DallasTemperature thermometer;
  String deviceAddressToString(DeviceAddress deviceAddress);
  TaskHandle_t termoThreadHandler;

  static void threadFunction(void * pvParameters);

};
