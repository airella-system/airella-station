#pragma once

#include <Arduino.h>
#include "communication/bluetooth/Bluetooth.h"
#include "maintenance/Statistics.h"

#define ACCESS_TOKEN_EXPIRATION_TIME 600000  // 1000ms * 60 * 10 = 10 min

struct RegistrationResult {
  bool ok = true;
  const char* message = "";
};

class ApiClass {
 public:
  ApiClass(){};
  ~ApiClass(){};

  RegistrationResult* registerStation();
  bool isRegistered();
  bool isAuth();
  bool publishMeasurement(String sensor, double value, bool authCheck = true);
  bool publishName(const char* name, bool authCheck = true);
  bool publishLocation(double longitude, double latitude, bool authCheck = true);
  bool publishAddress(const char* country, const char* city, const char* street, const char* number,
                      bool authCheck = true);

 private:
  bool updateAccessToken();
  bool registerSensor(const char* type);
  bool doRegister(RegistrationResult* result);
  bool doStationName(RegistrationResult* result);
  bool doStationAddress(RegistrationResult* result);
  bool doStationLocation(RegistrationResult* result);
  bool doTempSensor(RegistrationResult* result);
  bool doHumiditySensor(RegistrationResult* result);
  bool doPreasurreSensor(RegistrationResult* result);
  bool doPM1Sensor(RegistrationResult* result);
  bool doPM2_5Sensor(RegistrationResult* result);
  bool doPM10Sensor(RegistrationResult* result);
  bool addStatistics(RegistrationResult* result);
  bool doBtMacValue(RegistrationResult* result);
  bool addBtMacValue();
  void logRequest(const char* name, Http::Response& response);
  bool logRegistrationFail(const char* message, RegistrationResult* result);

  String accessToken = String("");
  unsigned long accessTokenMillis = 0;
};

extern ApiClass Api;
