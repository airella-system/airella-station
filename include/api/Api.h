#pragma once

#include <Arduino.h>
#define ACCESS_TOKEN_EXPIRATION_TIME 600000  // 1000ms * 60 * 10 = 10 min

class ApiClass {
public:
  ApiClass() {};
  ~ApiClass() {};

  bool registerStation();
  bool isRegistered();
  void configUpdated();
  bool publishMeasurement(String sensor, double value);
  bool publishName(const char *name);
  bool publishLocation(double longitude, double latitude);
  bool publishAddress(const char *country, const char *city, const char *street, const char *number);

private:
  bool updateAccessToken();
  bool registerSensor(const char *type);
  bool checkAuth();

  String accessToken = String("");
  unsigned long accessTokenMillis = 0;
};

extern ApiClass Api;