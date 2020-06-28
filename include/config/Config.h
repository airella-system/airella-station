#pragma once

#include <Arduino.h>

class Config {

public:
  enum InternetConnectionType {
    WIFI = 0,
    GSM = 1,
  };

  void load();
  void save();
  void reset();

  String getDevicePassword();
  InternetConnectionType getInternetConnectionType();
  String getWifiSsid();
  String getWifiPassword();
  String getRegistratonToken();
  String getRefreshToken();
  String getApiUrl();
  String getApiStationId();
  String getStationName();
  String getAddressCountry();
  String getAddressCity();
  String getAddressStreet();
  String getAddressNumber();

  void setDevicePassword(String devicePassword);
  void setInternetConnectionType(InternetConnectionType internetConnectionType);
  void setWifiSsid(String wifiSsid);
  void setWifiPassword(String wifiPassword);
  void setRegistratonToken(String registratonToken);
  void setRefreshToken(String refreshToken);
  void setApiUrl(String apiUrl);
  void setApiStationId(String apiStationId);
  void setStationName(String name);
  void setAddressCountry(String country);
  void setAddressCity(String city);
  void setAddressStreet(String street);
  void setAddressNumber(String number);

  static Config &instance() {
    static Config INSTANCE;
    return INSTANCE;
  }

private:
  Config(){};
  String devicePassword;
  InternetConnectionType internetConnectionType;
  String wifiSsid;
  String wifiPassword;
  String registratonToken;
  String refreshToken;
  String apiUrl;
  String apiStationId;
  String stationName;
  String addressCountry;
  String addressCity;
  String addressStreet;
  String addressNumber;
};