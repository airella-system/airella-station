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

Config::RegistrationState Config::registrationState = Config::RegistrationState::NO_REGISTRATION;

void Config::lock() {
  xSemaphoreTake(Config::mux, portMAX_DELAY);
}

void Config::unlock() {
  xSemaphoreGive(Config::mux);
}

void Config::load(bool lock /* = true */) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
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
  Config::registrationState = static_cast<Config::RegistrationState>(Config::preferences.getInt("reg-state", 0));
  Config::internetConnectionType =
      static_cast<Config::InternetConnectionType>(Config::preferences.getInt("inet-conn", 0));
  Config::preferences.end();
  if (lock) Config::unlock();
  save();
}

void Config::save(bool lock /* = true */) {
  Logger::debug("[Config::save()] Saving config to flash");
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
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
  Config::preferences.putInt("reg-state", static_cast<int>(Config::getRegistrationState()));
  Config::preferences.putInt("inet-conn", static_cast<int>(Config::getInternetConnectionType()));
  Config::preferences.end();
  if (lock) Config::unlock();
  Logger::debug("[Config::save()] Config saved");
}

void Config::reset(bool lock /* = true */) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::preferences.clear();
  Config::preferences.end();
  Config::load(false);
  if (lock) Config::unlock();
}

Config::InternetConnectionType Config::getInternetConnectionType(bool lock /* = true */) {
  if (lock) Config::lock();
  InternetConnectionType value = Config::internetConnectionType;
  if (lock) Config::unlock();
  return value;
}

String Config::getWifiSsid(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::wifiSsid;
  if (lock) Config::unlock();
  return value;
}

String Config::getWifiPassword(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::wifiPassword;
  if (lock) Config::unlock();
  return value;
}

String Config::getRegistratonToken(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::registratonToken;
  if (lock) Config::unlock();
  return value;
}

String Config::getRefreshToken(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::refreshToken;
  if (lock) Config::unlock();
  return value;
}

String Config::getApiUrl(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::apiUrl;
  if (lock) Config::unlock();
  return value;
}

String Config::getApiStationId(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::apiStationId;
  if (lock) Config::unlock();
  return value;
}

Config::RegistrationState Config::getRegistrationState(bool lock /* = true */) {
  if (lock) Config::lock();
  RegistrationState value = Config::registrationState;
  if (lock) Config::unlock();
  return value;
}

String Config::getStationName(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::stationName;
  if (lock) Config::unlock();
  return value;
}

String Config::getAddressCountry(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::addressCountry;
  if (lock) Config::unlock();
  return value;
}

String Config::getAddressCity(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::addressCity;
  if (lock) Config::unlock();
  return value;
}

String Config::getAddressStreet(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::addressStreet;
  if (lock) Config::unlock();
  return value;
}

String Config::getAddressNumber(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::addressNumber;
  if (lock) Config::unlock();
  return value;
}

String Config::getLocationLatitude(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::locationLatitude;
  if (lock) Config::unlock();
  return value;
}

String Config::getLocationLongitude(bool lock /* = true */) {
  if (lock) Config::lock();
  String value = Config::locationLongitude;
  if (lock) Config::unlock();
  return value;
}

bool Config::getLocationManual(bool lock /* = true */) {
  if (lock) Config::lock();
  bool value = Config::locationManual;
  if (lock) Config::unlock();
  return value;
}

void Config::setDevicePassword(String devicePassword, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::devicePassword = devicePassword;
  if (lock) Config::unlock();
}

void Config::setInternetConnectionType(Config::InternetConnectionType internetConnectionType, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::internetConnectionType = internetConnectionType;
  if (lock) Config::unlock();
}

void Config::setWifiSsid(String wifiSsid, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::wifiSsid = wifiSsid;
  if (lock) Config::unlock();
}

void Config::setWifiPassword(String wifiPassword, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::wifiPassword = wifiPassword;
  if (lock) Config::unlock();
}

void Config::setRegistratonToken(String registratonToken, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::registratonToken = registratonToken;
  if (lock) Config::unlock();
}

void Config::setRefreshToken(String refreshToken, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::refreshToken = refreshToken;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("refresh-token", refreshToken);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setApiUrl(String apiUrl, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::apiUrl = apiUrl;
  if (lock) Config::unlock();
}

void Config::setApiStationId(String apiStationId, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::apiStationId = apiStationId;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("api-station-id", apiStationId);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setRegistrationState(Config::RegistrationState registrationState, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::preferences.putInt("reg-state", static_cast<int>(registrationState));
  Config::preferences.end();
  Config::registrationState = registrationState;
  if (lock) Config::unlock();
}

void Config::setStationName(String stationName, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::stationName = stationName;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("station-name", stationName);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setAddressCountry(String addressCountry, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::addressCountry = addressCountry;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("address-country", addressCountry);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setAddressCity(String addressCity, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::addressCity = addressCity;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("address-city", addressCity);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setAddressStreet(String addressStreet, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::addressStreet = addressStreet;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("address-street", addressStreet);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setAddressNumber(String addressNumber, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::addressNumber = addressNumber;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("address-number", addressNumber);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setLocationLatitude(String latitude, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::locationLatitude = latitude;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("loc-latitude", latitude);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setLocationLongitude(String longitude, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::locationLongitude = longitude;
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("loc-longitude", longitude);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setLocationManual(bool manual, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::locationManual = manual;
  Config::preferences.begin("prefs", false);
  Config::preferences.putBool("loc-manual", manual);
  Config::preferences.end();
  if (lock) Config::unlock();
}
