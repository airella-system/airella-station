#include "config/Config.h"
#include "maintenance/Logger.h"

Preferences Config::preferences = Preferences();
SemaphoreHandle_t Config::mux = xSemaphoreCreateMutex();

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
String Config::accessToken = String();
String Config::gsmExtenderUrl = String();

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
  Config::accessToken = Config::preferences.getString("access-token", "");
  // Config::gsmExtenderUrl = Config::preferences.getString("gsmExtenderUrl", "https://gsm-extender.airella.cyfrogen.com/");
  Config::gsmExtenderUrl = Config::preferences.getString("gsmExtenderUrl", "https://airella-gsm-http-extender-prox.herokuapp.com/");
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::save(bool lock /* = true */) {
  Logger::debug("[Config::save()] Saving config to flash");
  if (lock) Config::lock();
  Config::preferences.begin("prefs", false);
  Config::preferences.putString("wifi-ssid", Config::getWifiSsid(false));
  Config::preferences.putString("wifi-password", Config::getWifiPassword(false));
  Config::preferences.putString("register-token", Config::getRegistratonToken(false));
  Config::preferences.putString("refresh-token", Config::getRefreshToken(false));
  Config::preferences.putString("api-url", Config::getApiUrl(false));
  Config::preferences.putString("api-station-id", Config::getApiStationId(false));
  Config::preferences.putString("station-name", Config::getStationName(false));
  Config::preferences.putString("address-country", Config::getAddressCountry(false));
  Config::preferences.putString("address-city", Config::getAddressCity(false));
  Config::preferences.putString("address-street", Config::getAddressStreet(false));
  Config::preferences.putString("address-number", Config::getAddressNumber(false));
  Config::preferences.putString("loc-latitude", Config::getLocationLatitude(false));
  Config::preferences.putString("loc-longitude", Config::getLocationLongitude(false));
  Config::preferences.putBool("loc-manual", Config::getLocationManual(false));
  Config::preferences.putInt("reg-state", static_cast<int>(Config::getRegistrationState(false)));
  Config::preferences.putInt("inet-conn", static_cast<int>(Config::getInternetConnectionType(false)));
  Config::preferences.putString("access-token", Config::getAccessToken(false));
  Config::preferences.putString("gsmExtenderUrl", Config::getGsmExtenderUrl(false));
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

String Config::getAtomicString(String* var, bool lock) {
  if (lock) Config::lock();
  String value = String(*var);
  if (lock) Config::unlock();
  return value;
}

void Config::syncValueWithFlash(String* newValue, String* configValue, const char* prefsName, bool lock) {
  if (lock) Config::lock();
  *configValue = String(*newValue);
  Config::preferences.begin("prefs", false);
  Config::preferences.putString(prefsName, *newValue);
  Config::preferences.end();
  if (lock) Config::unlock();
}

Config::InternetConnectionType Config::getInternetConnectionType(bool lock /* = true */) {
  if (lock) Config::lock();
  InternetConnectionType value = Config::internetConnectionType;
  if (lock) Config::unlock();
  return value;
}

/**
 * getters
*/

String Config::getWifiSsid(bool lock /* = true */) {
  return getAtomicString(&Config::wifiSsid, lock);
}

String Config::getWifiPassword(bool lock /* = true */) {
  return getAtomicString(&Config::wifiPassword, lock);
}

String Config::getRegistratonToken(bool lock /* = true */) {
  return getAtomicString(&Config::registratonToken, lock);
}

String Config::getRefreshToken(bool lock /* = true */) {
  return getAtomicString(&Config::refreshToken, lock);
}

String Config::getApiUrl(bool lock /* = true */) {
  return getAtomicString(&Config::apiUrl, lock);
}

String Config::getApiStationId(bool lock /* = true */) {
  return getAtomicString(&Config::apiStationId, lock);
}

Config::RegistrationState Config::getRegistrationState(bool lock /* = true */) {
  if (lock) Config::lock();
  RegistrationState value = Config::registrationState;
  if (lock) Config::unlock();
  return value;
}

String Config::getStationName(bool lock /* = true */) {
  return getAtomicString(&Config::stationName, lock);
}

String Config::getAddressCountry(bool lock /* = true */) {
  return getAtomicString(&Config::addressCountry, lock);
}

String Config::getAddressCity(bool lock /* = true */) {
  return getAtomicString(&Config::addressCity, lock);
}

String Config::getAddressStreet(bool lock /* = true */) {
  return getAtomicString(&Config::addressStreet, lock);
}

String Config::getAddressNumber(bool lock /* = true */) {
  return getAtomicString(&Config::addressNumber, lock);
}

String Config::getLocationLatitude(bool lock /* = true */) {
  return getAtomicString(&Config::locationLatitude, lock);
}

String Config::getLocationLongitude(bool lock /* = true */) {
  return getAtomicString(&Config::locationLongitude, lock);
}

bool Config::getLocationManual(bool lock /* = true */) {
  if (lock) Config::lock();
  bool value = Config::locationManual;
  if (lock) Config::unlock();
  return value;
}

String Config::getAccessToken(bool lock /* = true */) {
  return getAtomicString(&Config::accessToken, lock);
}

String Config::getGsmExtenderUrl(bool lock /* = true */) {
  return getAtomicString(&Config::gsmExtenderUrl, lock);
}

/**
 * setters
*/

void Config::setInternetConnectionType(Config::InternetConnectionType internetConnectionType, bool lock /* = true */) {
  Config::internetConnectionType = internetConnectionType;
  Config::preferences.begin("prefs", false);
  Config::preferences.putInt("inet-conn", static_cast<int>(internetConnectionType));
  Config::preferences.end();
  Internet::setType(static_cast<Internet::Type>(internetConnectionType));
  if (lock) Config::unlock();
}

void Config::setWifiSsid(String wifiSsid, bool lock /* = true */) {
  syncValueWithFlash(&wifiSsid, &Config::wifiSsid, "wifi-ssid", lock);
}

void Config::setWifiPassword(String wifiPassword, bool lock /* = true */) {
  syncValueWithFlash(&wifiPassword, &Config::wifiPassword, "wifi-password", lock);
}

void Config::setRegistratonToken(String registratonToken, bool lock /* = true */) {
  syncValueWithFlash(&registratonToken, &Config::registratonToken, "register-token", lock);
}

void Config::setRefreshToken(String refreshToken, bool lock /* = true */) {
  syncValueWithFlash(&refreshToken, &Config::refreshToken, "refresh-token", lock);
}

void Config::setApiUrl(String apiUrl, bool lock /* = true */) {
  syncValueWithFlash(&apiUrl, &Config::apiUrl, "api-url", lock);
}

void Config::setApiStationId(String apiStationId, bool lock /* = true */) {
  syncValueWithFlash(&apiStationId, &Config::apiStationId, "api-station-id", lock);
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
  syncValueWithFlash(&stationName, &Config::stationName, "station-name", lock);
}

void Config::setAddressCountry(String addressCountry, bool lock /* = true */) {
  syncValueWithFlash(&addressCountry, &Config::addressCountry, "address-country", lock);
}

void Config::setAddressCity(String addressCity, bool lock /* = true */) {
  syncValueWithFlash(&addressCity, &Config::addressCity, "address-city", lock);
}

void Config::setAddressStreet(String addressStreet, bool lock /* = true */) {
  syncValueWithFlash(&addressStreet, &Config::addressStreet, "address-street", lock);
}

void Config::setAddressNumber(String addressNumber, bool lock /* = true */) {
  syncValueWithFlash(&addressNumber, &Config::addressNumber, "address-number", lock);
}

void Config::setLocationLatitude(String latitude, bool lock /* = true */) {
  syncValueWithFlash(&latitude, &Config::locationLatitude, "loc-latitude", lock);
}

void Config::setLocationLongitude(String longitude, bool lock /* = true */) {
  syncValueWithFlash(&longitude, &Config::locationLongitude, "loc-longitude", lock);
}

void Config::setLocationManual(bool manual, bool lock /* = true */) {
  if (lock) Config::lock();
  Config::locationManual = manual;
  Config::preferences.begin("prefs", false);
  Config::preferences.putBool("loc-manual", manual);
  Config::preferences.end();
  if (lock) Config::unlock();
}

void Config::setAccessToken(String accessToken, bool lock /* = true */) {
  syncValueWithFlash(&accessToken, &Config::accessToken, "access-token", lock);
}

void Config::setGsmExtenderUrl(String gsmExtenderUrl, bool lock /* = true */) {
  syncValueWithFlash(&gsmExtenderUrl, &Config::gsmExtenderUrl, "gsmExtenderUrl", lock);
}
