#pragma once

#include <Arduino.h>
#define ACCESS_TOKEN_EXPIRATION_TIME 600000 // 1000ms * 60 * 10 = 10 min

class ApiClass {
public:
  ApiClass() { this->accessTokenMillis = 0; }

  bool registerStation();
  bool publishMeasurement(String sensor, double value);
  bool publishLocation(double longitude, double latitude);
  bool isRegistered();
  void configUpdated();

private:
  bool updateAccessToken();
  bool registerSensor(const char *type);

  String accessToken;
  unsigned long accessTokenMillis;
};

extern ApiClass Api;