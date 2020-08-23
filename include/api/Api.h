#pragma once

#include <Arduino.h>
#define ACCESS_TOKEN_EXPIRATION_TIME 600000  // 1000ms * 60 * 10 = 10 min

struct RegistrationResult {
  bool ok = true;
  const char* message = "";
};

class ApiClass {
public:
  ApiClass() {};
  ~ApiClass() {};

  RegistrationResult* registerStation();
  bool isRegistered();
  bool isAuth();
  bool publishMeasurement(String sensor, double value, bool authCheck = true);
  bool publishName(const char *name, bool authCheck = true);
  bool publishLocation(double longitude, double latitude, bool authCheck = true);
  bool publishAddress(const char *country, const char *city, const char *street, const char *number, bool authCheck = true);

private:
  bool updateAccessToken();
  bool registerSensor(const char *type);

  String accessToken = String("");
  unsigned long accessTokenMillis = 0;
};

extern ApiClass Api;