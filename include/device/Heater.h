#pragma once

#include <DallasTemperature.h>
#include <analogWrite.h>
#include "device/Sensor.h"
#include "device/WeatherSensor.h"

#define ONE_WIRE_MAX_DEV 2
#define CALCULATE_DEWPOIN 1

struct HeaterStatus {
  bool heaterIsOn;
  bool threadIsRunning;
};

class Heater : public Sensor {
public:
  Heater(WeatherSensor &_weatherSensor);
  ~Heater();
  void on();
  void off();
  void run();
  void stop();
  float getTemperature();
  float getHumidity();
  float dewPoint(float humidity, float temperature) const;
  HeaterStatus getHeaterState() const;
  HeaterStatus heaterStatus;
  const float temperatureTrashold = 3;
  const float temperatureLevel = 0;

private:
  HeaterConfig config;
  float temperatureDevice[ONE_WIRE_MAX_DEV];
  DeviceAddress deviceAddress[ONE_WIRE_MAX_DEV];
  int numberOfDevices;
  OneWire communicationBus;
  DallasTemperature thermometer;
  TaskHandle_t termoThreadHandler;
  WeatherSensor weatherSensor;
  String deviceAddressToString(DeviceAddress deviceAddress) const;
  bool shouldTurnOn() const;
  bool shouldTurnOff() const;

  static void threadFunction(void * pvParameters);

};
