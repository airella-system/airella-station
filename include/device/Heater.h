#pragma once

#include <DallasTemperature.h>
#include <analogWrite.h>
#include "device/Sensor.h"
#include "device/WeatherSensor.h"

#define ONE_WIRE_MAX_DEV 2
#define MAX_POWER 255
// #define CALCULATE_DEWPOIN true

struct HeaterStatus {
  bool heaterIsOn;
  bool threadIsRunning;
};

class Heater : public Sensor {
public:
  Heater(WeatherSensor &_weatherSensor);
  ~Heater();
  void on();
  void setPower(int power);
  void off();
  void run();
  void stop();
  float getTemperature();
  float getHumidity();
  float dewPoint(float humidity, float temperature) const;
  unsigned int getCurrentPower() const;
  HeaterStatus getHeaterState() const;
  HeaterStatus heaterStatus;
  const float temperatureTrashold = 1;
  const float temperatureLevel = 20;

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
  unsigned int currentPower;

  static void threadFunction(void * pvParameters);

};
