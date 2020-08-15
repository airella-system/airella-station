#include "config/Config.h"
#include "maintenance/Logger.h"

Preferences Config::preferences = Preferences();
SemaphoreHandle_t Config::mux = xSemaphoreCreateMutex();

String Config::devicePassword = String();
Config::InternetConnectionType Config::internetConnectionType = Config::InternetConnectionType::WIFI;
String Config::wifiSsid = String();
String Config::wifiPassword = String();
String Config::registratonToken = String();
String Config::refreshToken = String();
String Config::apiUrl = String();
String Config::apiStationId = String();
String Config::stationName = String();
String Config::addressCountry = String();
String Config::addressCity = String();
String Config::addressStreet = String();
String Config::addressNumber = String();
String Config::locationLatitude = String();
String Config::locationLongitude = String();
bool Config::locationManual = false;

String Config::chunkerTestUpload = String();
String Config::chunkerTestDownload = String();

Config::RegistrationState Config::registrationState = Config::RegistrationState::NO_REGISTRATION;

void Config::lock() {
  xSemaphoreTake(Config::mux, portMAX_DELAY);
}

void Config::unlock() {
  xSemaphoreGive(Config::mux);
}

void Config::load(bool lock) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::devicePassword = Config::preferences.getString("device-password", "");
  Config::internetConnectionType =
      static_cast<Config::InternetConnectionType>(Config::preferences.getInt("inet-conn", 0));
  Config::wifiSsid = Config::preferences.getString("wifi-ssid", "");
  Config::wifiPassword = Config::preferences.getString("wifi-password", "");
  Config::registratonToken = Config::preferences.getString("register-token", "");
  Config::refreshToken = Config::preferences.getString("refresh-token", "");
  Config::apiUrl = Config::preferences.getString("api-url", "http://airella.cyfrogen.com/api");
  Config::apiStationId = Config::preferences.getString("api-station-id", "");
  Config::stationName = Config::preferences.getString("station-name", "");
  Config::addressCountry = Config::preferences.getString("address-country", "");
  Config::addressCity = Config::preferences.getString("address-city", "");
  Config::addressStreet = Config::preferences.getString("address-street", "");
  Config::addressNumber = Config::preferences.getString("address-number", "");
  Config::locationLatitude = Config::preferences.getString("loc-latitude", "");
  Config::locationLongitude = Config::preferences.getString("loc-longitude", "");
  Config::locationManual = Config::preferences.getBool("loc-manual", false);
  if (!Config::getRefreshToken().equals("") && !Config::getApiStationId().equals("")) {
    Config::registrationState = Config::RegistrationState::REGISTERED;
  } else {
    Config::registrationState = Config::RegistrationState::NO_REGISTRATION;
  }
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::save(bool lock) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("device-password", Config::getDevicePassword());
  Config::preferences.putString("wifi-ssid", Config::getWifiSsid());
  Config::preferences.putString("wifi-password", Config::getWifiPassword());
  Config::preferences.putString("register-token", Config::getRegistratonToken());
  Config::preferences.putString("refresh-token", Config::getRefreshToken());
  Config::preferences.putString("api-url", Config::getApiUrl());
  Config::preferences.putString("api-station-id", Config::getApiStationId());
  Config::preferences.putString("station-name", Config::getStationName());
  Config::preferences.putString("address-country", Config::getAddressCountry());
  Config::preferences.putString("address-city", Config::getAddressCity());
  Config::preferences.putString("address-street", Config::getAddressStreet());
  Config::preferences.putString("address-number", Config::getAddressNumber());
  Config::preferences.putString("loc-latitude", Config::getLocationLatitude());
  Config::preferences.putString("loc-longitude", Config::getLocationLongitude());
  Config::preferences.putBool("loc-manual", Config::getLocationManual());
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::reset(bool lock) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::preferences.clear();
  Config::preferences.end();
  Config::load(false);
  if (lock) Config::unlock();
}

String Config::getDevicePassword(bool lock) {
  return Config::devicePassword;
}

Config::InternetConnectionType Config::getInternetConnectionType(bool lock) {
  return Config::internetConnectionType;
}

String Config::getWifiSsid(bool lock) {
  return Config::wifiSsid;
}

String Config::getWifiPassword(bool lock) {
  return Config::wifiPassword;
}

String Config::getRegistratonToken(bool lock) {
  return Config::registratonToken;
}

String Config::getRefreshToken(bool lock) {
  return Config::refreshToken;
}

String Config::getApiUrl(bool lock) {
  return Config::apiUrl;
}

String Config::getApiStationId(bool lock) {
  return Config::apiStationId;
}

Config::RegistrationState Config::getRegistrationState(bool lock) {
  return Config::registrationState;
}

String Config::getStationName(bool lock) {
  return Config::stationName;
}

String Config::getAddressCountry(bool lock) {
  return Config::addressCountry;
}

String Config::getAddressCity(bool lock) {
  return Config::addressCity;
}

String Config::getAddressStreet(bool lock) {
  return Config::addressStreet;
}

String Config::getAddressNumber(bool lock) {
  return Config::addressNumber;
}

String Config::getLocationLatitude(bool lock) {
  return Config::locationLatitude;
}

String Config::getLocationLongitude(bool lock) {
  return Config::locationLongitude;
}

bool Config::getLocationManual(bool lock) {
  return Config::locationManual;
}

String Config::getChunkerTestUpload(bool lock) {
  return Config::chunkerTestUpload;
}

String Config::getChunkerTestDownload(bool lock) {
  return Config::chunkerTestDownload;
}

void Config::setDevicePassword(String devicePassword, bool lock) {
  if (lock) Config::lock();
  Config::devicePassword = devicePassword;
  if (lock) Config::unlock();
}

void Config::setInternetConnectionType(Config::InternetConnectionType internetConnectionType, bool lock) {
  if (lock) Config::lock();
  Config::internetConnectionType = internetConnectionType;
  if (lock) Config::unlock();
}

void Config::setWifiSsid(String wifiSsid, bool lock) {
  if (lock) Config::lock();
  Config::wifiSsid = wifiSsid;
  if (lock) Config::unlock();
}

void Config::setWifiPassword(String wifiPassword, bool lock) {
  if (lock) Config::lock();
  Config::wifiPassword = wifiPassword;
  if (lock) Config::unlock();
}

void Config::setRegistratonToken(String registratonToken, bool lock) {
  if (lock) Config::lock();
  Config::registratonToken = registratonToken;
  if (lock) Config::unlock();
}

void Config::setRefreshToken(String refreshToken, bool lock) {
  if (lock) Config::lock();
  Config::refreshToken = refreshToken;
  if (lock) Config::unlock();
}

void Config::setApiUrl(String apiUrl, bool lock) {
  if (lock) Config::lock();
  Config::apiUrl = apiUrl;
  if (lock) Config::unlock();
}

void Config::setApiStationId(String apiStationId, bool lock) {
  if (lock) Config::lock();
  Config::apiStationId = apiStationId;
  if (lock) Config::unlock();
}

void Config::setRegistrationState(Config::RegistrationState registrationState, bool lock) {
  if (lock) Config::lock();
  Config::registrationState = registrationState;
  if (lock) Config::unlock();
}

void Config::setStationName(String stationName, bool lock) {
  if (lock) Config::lock();
  Config::stationName = stationName;
  if (lock) Config::unlock();
}

void Config::setAddressCountry(String addressCountry, bool lock) {
  if (lock) Config::lock();
  Config::addressCountry = addressCountry;
  if (lock) Config::unlock();
}

void Config::setAddressCity(String addressCity, bool lock) {
  if (lock) Config::lock();
  Config::addressCity = addressCity;
  if (lock) Config::unlock();
}

void Config::setAddressStreet(String addressStreet, bool lock) {
  if (lock) Config::lock();
  Config::addressStreet = addressStreet;
  if (lock) Config::unlock();
}

void Config::setAddressNumber(String addressNumber, bool lock) {
  if (lock) Config::lock();
  Config::addressNumber = addressNumber;
  if (lock) Config::unlock();
}

void Config::setLocationLatitude(String latitude, bool lock) {
  if (lock) Config::lock();
  Config::locationLatitude = latitude;
  if (lock) Config::unlock();
}

void Config::setLocationLongitude(String longitude, bool lock) {
  if (lock) Config::lock();
  Config::locationLongitude = longitude;
  if (lock) Config::unlock();
}

void Config::setLocationManual(bool manual, bool lock) {
  if (lock) Config::lock();
  Config::locationManual = manual;
  if (lock) Config::unlock();
}

void Config::setChunkerTestUpload(String manual, bool lock) {
  if (lock) Config::lock();
  Config::chunkerTestUpload = manual;
  if (lock) Config::unlock();
}

void Config::setChunkerTestDownload(String manual, bool lock) {
  if (lock) Config::lock();
  Config::chunkerTestDownload = manual;
  if (lock) Config::unlock();
}
