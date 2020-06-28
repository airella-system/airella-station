#include "config/Config.h"

#include <Preferences.h>
Preferences preferences;
void Config::load() {
  preferences.begin("prefs", false);
  this->devicePassword = preferences.getString("dev-pass", "");
  this->internetConnectionType = static_cast<Config::InternetConnectionType>(
      preferences.getInt("inet-conn", 0));
  this->wifiSsid = preferences.getString("wifi-ssid", "");
  this->wifiPassword = preferences.getString("wifi-password", "");
  this->registratonToken = preferences.getString("registration-token", "");
  this->refreshToken = preferences.getString("refresh-token", "");
  this->apiUrl =
      preferences.getString("api-url", "http://airella.cyfrogen.com/api");
  this->apiStationId = preferences.getString("api-station-id", "");
  this->stationName = preferences.getString("station-name", "");
  this->addressCountry = preferences.getString("address-country", "");
  this->addressCity = preferences.getString("address-city", "");
  this->addressStreet = preferences.getString("address-street", "");
  this->addressNumber = preferences.getString("address-number", "");

  preferences.end();
}

void Config::save() {
  preferences.begin("prefs", false);
  preferences.putString("device-password", Config::getDevicePassword());
  preferences.putString("wifi-ssid", Config::getWifiSsid());
  preferences.putString("wifi-password", Config::getWifiPassword());
  preferences.putString("registration-token", Config::getRegistratonToken());
  preferences.putString("refresh-token", Config::getRefreshToken());
  preferences.putString("api-url", Config::getApiUrl());
  preferences.putString("api-station-id", Config::getApiStationId());
  preferences.putString("station-name", Config::getStationName());
  preferences.putString("address-country", Config::getAddressCountry());
  preferences.putString("address-city", Config::getAddressCity());
  preferences.putString("address-street", Config::getAddressStreet());
  preferences.putString("address-number", Config::getAddressNumber());
  preferences.end();
}

void Config::reset() {
  preferences.begin("prefs", false);
  preferences.clear();
  preferences.end();
  Config::instance().load();
}

String Config::getDevicePassword() { return this->devicePassword; }

Config::InternetConnectionType Config::getInternetConnectionType() {
  return this->internetConnectionType;
}

String Config::getWifiSsid() { return this->wifiSsid; }

String Config::getWifiPassword() { return this->wifiPassword; }

String Config::getRegistratonToken() { return this->registratonToken; }

String Config::getRefreshToken() { return this->refreshToken; }

String Config::getApiUrl() { return this->apiUrl; }

String Config::getApiStationId() { return this->apiStationId; }

void Config::setDevicePassword(String devicePassword) {
  this->devicePassword = devicePassword;
}

void Config::setInternetConnectionType(
    Config::InternetConnectionType internetConnectionType) {
  this->internetConnectionType = internetConnectionType;
}

void Config::setWifiSsid(String wifiSsid) { this->wifiSsid = wifiSsid; }

void Config::setWifiPassword(String wifiPassword) {
  this->wifiPassword = wifiPassword;
}

void Config::setRegistratonToken(String registratonToken) {
  this->registratonToken = registratonToken;
}

void Config::setRefreshToken(String refreshToken) {
  this->refreshToken = refreshToken;
}

void Config::setApiUrl(String apiUrl) { this->apiUrl = apiUrl; }

void Config::setApiStationId(String apiStationId) {
  this->apiStationId = apiStationId;
}
