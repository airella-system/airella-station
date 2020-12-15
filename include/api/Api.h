#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include "communication/bluetooth/Bluetooth.h"
#include "maintenance/Statistics.h"
#include "api/entity/RegiserModel.h"
#include "api/entity/DataModel.h"

#define ACCESS_TOKEN_EXPIRATION_TIME 600000  // 1000ms * 60 * 10 = 10 min

struct RegistrationResult {
  bool ok = true;
  const char* message = "";
};

class ApiClass {
 public:
  ApiClass() {};
  ~ApiClass() {};

  RegistrationResult registerStation();
  bool isRegistered();
  bool isAuth();
  bool publishMeasurement(String sensor, double value, bool authCheck = true);
  bool publishName(const char *name, bool authCheck = true);
  bool publishLocation(double longitude, double latitude, bool authCheck = true);
  bool publishAddress(
    const char* country,
    const char* city, 
    const char* street, 
    const char* number,
    bool authCheck = true
  );
  bool publishDataModel(const String& body);

 private:
  bool updateAccessToken();
  bool addBtMacValue();
  void logRequest(const char* name, Http::Response& response);
  bool noInternetConnectionOptimalization();
  int tryCount = 0;
  long lastTryTime = 0;

  String accessToken = String("");
  unsigned long accessTokenMillis = 0;
};

extern ApiClass Api;
