#include "api/Api.h"
#include <ArduinoJson.h>
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

/**
 * obsłużyć error w trakcie rejestracji - niepełna rejestracja - jak dokończyć rejestrację
 * może podzielić rejestracje na oszczególne funkcje i wtedy wywoływać w zależności od ustawionego statusu
*/

RegistrationResult* ApiClass::registerStation() {
  RegistrationResult* result = new RegistrationResult();
  result->ok = false;
  if(isRegistered()) {
    Logger::debug("[ApiClass::registerStation()] Station already registered");
    result->message = "Station already registered";
    return result;
  }

  accessToken = String("");
  accessTokenMillis = 0;
  if (Config::getRegistratonToken().equals("")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("[ApiClass::registerStation()] Registration fail - no registration token");
    result->message = "No registration token";
    return result;
  }

  if (!Internet::isConnected()) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("[ApiClass::registerStation()] Registration fail - no internet connection");
    result->message = "No internet connection";
    return result;
  }
  Config::setRegistrationState(Config::RegistrationState::REGISTERING);

  String registrationToken = Config::getRegistratonToken();
  String apiUrlBase = Config::getApiUrl();
  String url = apiUrlBase + "/auth/register-station";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
  Logger::debug(registrationToken.c_str());
  doc["stationRegistrationToken"] = registrationToken.c_str();
  String body = "";
  serializeJson(doc, body);
  Logger::debug(body.c_str());

  Http::Response response = Internet::httpPost(url, body);

  String debugText = String("[ApiClass::registerStation()] Registraton response code: ") 
    + response.code 
    + " payload: " 
    + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code != 201) {
    Logger::debug(String("[ApiClass::registerStation()] Registration fail - internet error: " + response.code).c_str());
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = String("internet error: " + response.code).c_str();
    return result;
  }

  DynamicJsonDocument doc2(2 * JSON_OBJECT_SIZE(2) + 120);
  deserializeJson(doc2, response.payload);
  const char *id = doc2["data"]["id"];
  const char *refreshToken = doc2["data"]["refreshToken"];
  Config::setApiStationId(String(id));
  Config::setRefreshToken(String(refreshToken));

  if (!updateAccessToken()) {
    Logger::debug("[ApiClass::registerStation()] Unable to update token");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to update token";
    return result;
  }

  String name = Config::getStationName();
  if (!name.equals("")) {
    if (!publishName(name.c_str(), false)) {
      Logger::debug("[ApiClass::registerStation()] Unable to set station name");
      Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
      result->message = "Unable to set station name";
      return result;
    }
  }

  bool actionResult = true;
  actionResult = publishAddress(
    Config::getAddressCountry().c_str(), 
    Config::getAddressCity().c_str(),
    Config::getAddressStreet().c_str(), 
    Config::getAddressNumber().c_str(),
    false
  );

  if (!actionResult) {
    Logger::debug("[ApiClass::registerStation()] Unable to set address");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to set address";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Set station address");
  }

  actionResult = publishLocation(
    Config::getLocationLatitude().toDouble(),
    Config::getLocationLongitude().toDouble(),
    false
  );

  if (!actionResult) {
    Logger::debug("[ApiClass::registerStation()] Unable to set location");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to set location";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Set station location");
  }

  if (!registerSensor("temperature")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register remperature sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register remperature sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered remperature sensor");
  }
  
  if (!registerSensor("humidity")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register humidity sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register humidity sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered humidity sensor");
  }
  
  if (!registerSensor("pressure")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register pressure sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register pressure sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered pressure sensor");
  }

  if (!registerSensor("pm1")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register pm1 sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register pm1 sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered pm1 sensor");
  }

  if (!registerSensor("pm2_5")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register pm2_5 sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register pm2_5 sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered pm2_5 sensor");
  }

  if (!registerSensor("pm10")) {
    Logger::debug("[ApiClass::registerStation()] Unable to register pm10 sensor");
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result->message = "Unable to register pm10 sensor";
    return result;
  }
  else {
    Logger::info("[ApiClass::registerStation()] Registered pm10 sensor");
  }

  Logger::info("[ApiClass::registerStation()] Registered successfull.");
  Config::setRegistrationState(Config::RegistrationState::REGISTERED);
  Config::save();

  result->ok = true;
  return result;
}

bool ApiClass::isRegistered() {
  return Config::getRegistrationState() == Config::RegistrationState::REGISTERED;
}

bool ApiClass::registerSensor(const char *type) {
  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" + Config::getApiStationId() + "/sensors";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["type"] = type;
  doc["id"] = type;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor add response code: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 201;
}

bool ApiClass::updateAccessToken() {
  if (accessToken.equals("") || (millis() - accessTokenMillis) > ACCESS_TOKEN_EXPIRATION_TIME) {
    String registrationToken = Config::getRegistratonToken();
    String apiUrlBase = Config::getApiUrl();
    String url = apiUrlBase + "/auth/refresh-token";
    String refreshToken = Config::getRefreshToken();

    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
    doc["refreshToken"] = refreshToken.c_str();
    String body = "";
    serializeJson(doc, body);

    Http::Response response = Internet::httpPost(url, body);

    String debugText = String("Get token response code: ") + response.code + " payload: " + response.payload;
    Logger::debug(debugText.c_str());
    if (response.code == 200) {
      DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + 280);
      deserializeJson(doc, response.payload);
      const char *token = doc["data"]["accessToken"]["token"];
      accessToken = String(token);
      accessTokenMillis = millis();
      Logger::info("[ApiClass::updateAccessToken] Token refreshed successfully.");
      return true;
    }
    Logger::info("[ApiClass::updateAccessToken] Unable to refresh token.");
    return false;
  } else {
    return true;
  }
}

bool ApiClass::publishName(const char *name, bool authCheck /*= true*/) {
  if (authCheck && !isAuth()) {
    Logger::debug("[ApiClass::publishName()] Authorization failed");
    return false;
  }

  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" + Config::getApiStationId() + "/name";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
  doc["name"] = name;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("[ApiClass::publishName] Sensor set name response code: ") 
    + response.code 
    + " payload: " 
    + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::publishLocation(double latitude, double longitude, bool authCheck /*= true*/) {
  if (authCheck && !isAuth()) {
    Logger::debug("[ApiClass::publishLocation()] Authorization failed");
    return false;
  }

  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" + Config::getApiStationId() + "/location";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor set location response code: ") 
    + response.code 
    + " payload: " 
    + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::publishAddress(const char *country, const char *city, const char *street, const char *number, bool authCheck /*= true*/) {
  if (authCheck && !isAuth()) {
    Logger::debug("[ApiClass::publishAddress()] Authorization failed");
    return false;
  }

  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" + Config::getApiStationId() + "/address";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
  doc["country"] = country;
  doc["city"] = city;
  doc["street"] = street;
  doc["number"] = number;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor set location response code: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::publishMeasurement(String sensor, double value, bool authCheck /*= true*/) {
  if (authCheck && !isAuth()) {
    Logger::debug("[ApiClass::publishMeasurement()] Authorization failed");
    return false;
  }

  String apiUrlBase = Config::getApiUrl();
  String url = apiUrlBase + "/stations/" + Config::getApiStationId() + "/sensors/" + sensor + "/measurements";

  const size_t capacity = JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);
  Logger::debug(String(value).c_str());
  doc["value"] = value;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);

  String debugText = String("Add measurement response code: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::isAuth() {
  return isRegistered() && updateAccessToken();
}

ApiClass Api;
