#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "communication/common/Internet.h"
#include "config/Defines.h"

class Config {
 public:
  enum InternetConnectionType {
    WIFI = 0,
    GSM = 1,
  };

  enum RegistrationState {
    NO_REGISTRATION = 0,
    REGISTRATION_ERROR = 2,
    REGISTERED = 3
  };

  static void load(bool lock = true);
  static void save(bool lock = true);
  static void reset(bool lock = true);

  static InternetConnectionType getInternetConnectionType(bool lock = true);
  static String getWifiSsid(bool lock = true);
  static String getWifiPassword(bool lock = true);
  static String getRegistratonToken(bool lock = true);
  static String getRefreshToken(bool lock = true);
  static String getApiUrl(bool lock = true);
  static String getApiStationId(bool lock = true);
  static String getStationName(bool lock = true);
  static String getAddressCountry(bool lock = true);
  static String getAddressCity(bool lock = true);
  static String getAddressStreet(bool lock = true);
  static String getAddressNumber(bool lock = true);
  static String getLocationLatitude(bool lock = true);
  static String getLocationLongitude(bool lock = true);
  static bool getLocationManual(bool lock = true);
  static RegistrationState getRegistrationState(bool lock = true);
  static String getAccessToken(bool lock = true);
  static String getGsmExtenderUrl(bool lock = true);
  static long getPersistedTime(bool lock = true);
  static String getGsmConfig(bool lock = true);

  static void setInternetConnectionType(InternetConnectionType internetConnectionType, bool lock = true);
  static void setWifiSsid(String wifiSsid, bool lock = true);
  static void setWifiPassword(String wifiPassword, bool lock = true);
  static void setRegistratonToken(String registratonToken, bool lock = true);
  static void setRefreshToken(String refreshToken, bool lock = true);
  static void setApiUrl(String apiUrl, bool lock = true);
  static void setApiStationId(String apiStationId, bool lock = true);
  static void setStationName(String name, bool lock = true);
  static void setAddressCountry(String country, bool lock = true);
  static void setAddressCity(String city, bool lock = true);
  static void setAddressStreet(String street, bool lock = true);
  static void setAddressNumber(String number, bool lock = true);
  static void setLocationLatitude(String latitude, bool lock = true);
  static void setLocationLongitude(String longitude, bool lock = true);
  static void setLocationManual(bool manual, bool lock = true);
  static void setRegistrationState(RegistrationState registrationState, bool lock = true);
  static void setAccessToken(String accessToken, bool lock = true);
  static void setGsmExtenderUrl(String gsmExtenderUrl, bool lock = true);
  static void setPersistedTime(long persistedTime, bool lock = true);
  static void setGsmConfig(String gsmConfig, bool lock = true);

  static Preferences preferences;

 private:
  static SemaphoreHandle_t mux;

  static void lock();
  static void unlock();
  static String getAtomicString(String* value, bool lock);
  static void syncValueWithFlash(String* newValue, String* configValue, const char* prefsName, bool lock);

  static InternetConnectionType internetConnectionType;
  static String wifiSsid;
  static String wifiPassword;
  static String registratonToken;
  static String refreshToken;
  static String apiUrl;
  static String apiStationId;
  static String stationName;
  static String addressCountry;
  static String addressCity;
  static String addressStreet;
  static String addressNumber;
  static String locationLatitude;
  static String locationLongitude;
  static bool locationManual;
  static String accessToken;
  static RegistrationState registrationState;
  static String gsmExtenderUrl;
  static long persistedTime;
  static String gsmConfig;
};