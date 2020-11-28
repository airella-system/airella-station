#include "config/Autocofiguration.h"

void Autocofiguration::tryConfig() {
  if(isConfigured()) {
    Logger::debug("[Autocofiguration::tryConfig()] Already configured: OK");
    return;
  }

  Logger::debug("[Autocofiguration::tryConfig()] Search configuration file.");
  if(!configFileExists()) {
    Logger::debug("[Autocofiguration::tryConfig()] Not found configuration file.");
    return;
  }
  
  DynamicJsonDocument* configDoc = nullptr;
  if(!parseConfigFile(configDoc)) {
    Logger::debug("[Autocofiguration::tryConfig()] Unable to parse configuration file.");
    return;
  }

  if(!configFileIsCorect(configDoc)) {
    Logger::debug("[Autocofiguration::tryConfig()] Invalid configuration file structore.");
    return;
  }

  setConfig(configDoc);
  Logger::debug("[Autocofiguration::tryConfig()] Station configured via configuration file: OK");
}

bool Autocofiguration::isConfigured() {
  return Config::getRegistrationState() != Config::RegistrationState::NO_REGISTRATION;
}

bool Autocofiguration::configFileExists() {
  DeviceContainer.storage->tryToInit();
  return DeviceContainer.storage->getStorage()->exists(configFileName);
}

bool Autocofiguration::parseConfigFile(DynamicJsonDocument* configDoc) {
  Logger::debug("[Autocofiguration::parseConfigFile()] Reading configuration file.");
  Storage* storage = DeviceContainer.storage;
  unsigned int fileSize = storage->getFileSize(configFileName);
  if(fileSize > 10 * 1024) {
    Logger::debug("[Autocofiguration::parseConfigFile()] Unable to open, too large file.");
    return false;
  }
  configDoc = new DynamicJsonDocument(JSON_OBJECT_SIZE(20) + fileSize);
  String fileContent = DeviceContainer.storage->read(configFileName);
  deserializeJson(*configDoc, fileContent);
  return true;
}

bool Autocofiguration::configFileIsCorect(DynamicJsonDocument* configDoc) {
  const char* requiredKeys[20] = {
    "wifi-ssid",
    "wifi-password",
    "register-token",
    "refresh-token",
    "api-url",
    "api-station-id",
    "station-name",
    "address-country",
    "address-city",
    "address-street",
    "address-number",
    "loc-latitude",
    "loc-longitude",
    "loc-manual",
    "reg-state",
    "inet-conn",
    "access-token",
    "persistedTime",
    "gsm-ext-url",
    "gsm-config"
  };
  for(auto key : requiredKeys) {
    if(configDoc->containsKey(key)) return false;
  }
  return true;
}

void Autocofiguration::setConfig(DynamicJsonDocument* configDoc) {
  Logger::debug("[Autocofiguration::setConfig()] Starting configuration.");

  Config::setRegistrationState(static_cast<Config::RegistrationState>((*configDoc)["reg-state"].as<int>()), false);
  Config::setInternetConnectionType(static_cast<Config::InternetConnectionType>((*configDoc)["reg-state"].as<int>()), false);
  Config::setWifiSsid((*configDoc)["wifi-ssid"], false);
  Config::setWifiPassword((*configDoc)["wifi-password"], false);
  Config::setRegistratonToken((*configDoc)["register-token"], false);
  Config::setRefreshToken((*configDoc)["refresh-token"], false);
  Config::setApiUrl((*configDoc)["api-url"], false);
  Config::setApiStationId((*configDoc)["api-station-id"], false);
  Config::setStationName((*configDoc)["station-name"], false);
  Config::setAddressCountry((*configDoc)["address-country"], false);
  Config::setAddressCity((*configDoc)["address-city"], false);
  Config::setAddressStreet((*configDoc)["address-street"], false);
  Config::setAddressNumber((*configDoc)["address-number"], false);
  Config::setLocationLatitude((*configDoc)["loc-latitude"], false);
  Config::setLocationLongitude((*configDoc)["loc-longitude"], false);
  Config::setLocationManual((*configDoc)["loc-manual"], false);
  Config::setAccessToken((*configDoc)["access-token"], false);
  Config::setGsmExtenderUrl((*configDoc)["gsm-ext-url"], false);
  Config::setPersistedTime((*configDoc)["persistedTime"], false);
  Config::setGsmConfig((*configDoc)["gsm-config"], false);
  
  Logger::debug("[Autocofiguration::setConfig()] Configured.");
}
