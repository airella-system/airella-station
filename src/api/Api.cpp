#include "api/Api.h"
#include <ArduinoJson.h>
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"

RegistrationResult* ApiClass::registerStation() {
  RegistrationResult* result = new RegistrationResult();
  result->ok = true;
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

  Config::RegistrationState currentRegistrationState = Config::getRegistrationState();

  if(currentRegistrationState < Config::RegistrationState::REGISTERED) {
    if(!doRegister(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::STATION_NAME) {
    if(!doStationName(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::STATION_ADDRESS) {
    if(!doStationAddress(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::STATION_LOCATION) {
    if(!doStationLocation(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::TEMP_SENSOR) {
    if(!doTempSensor(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::HUMIDITY_SENSOR) {
    if(!doHumiditySensor(result)) return result;
  }
  
  if(currentRegistrationState < Config::RegistrationState::PREASSURE_SENSOR) {
    if(!doPreasurreSensor(result)) return result;
  }
  
  if(currentRegistrationState < Config::RegistrationState::PM1_SENSOR) {
    if(!doPM1Sensor(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::PM2_5_SENSOR) {
    if(!doPM2_5Sensor(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::PM10_SENSOR) {
    if(!doPM10Sensor(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::MAC_STATISTIC) {
    if(!doBtMacStatistic(result)) return result;
  }

  if(currentRegistrationState < Config::RegistrationState::MAC_VALUE) {
    if(!doBtMacValue(result)) return result;
  }

  Logger::info("[ApiClass::registerStation()] Registered successfull");
  Config::setRegistrationState(Config::RegistrationState::REGISTERED_OK);
  Config::save();

  return result;
}

bool ApiClass::doRegister(RegistrationResult* result) {
  String registrationToken = Config::getRegistratonToken();
  String apiUrlBase = Config::getApiUrl();
  String url = apiUrlBase + "/auth/register-station";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["stationRegistrationToken"] = registrationToken.c_str();
  String body = "";
  serializeJson(doc, body);
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
    result->ok = false;
    return false;
  }

  DynamicJsonDocument doc2(2 * JSON_OBJECT_SIZE(2) + 120);
  deserializeJson(doc2, response.payload);
  const char *id = doc2["data"]["id"];
  const char *refreshToken = doc2["data"]["refreshToken"];
  Config::setApiStationId(String(id));
  Config::setRefreshToken(String(refreshToken));
  Config::setRegistrationState(Config::RegistrationState::REGISTERED);

  if (!updateAccessToken()) {
    return logRegistrationFail("Unable to update token", result);
  }

  return true;
}

bool ApiClass::doStationName(RegistrationResult* result) {
  String name = Config::getStationName();
  if (name.equals("")) return true;

  if (publishName(name.c_str(), false)) {
    Logger::debug("[ApiClass::registerStation()] Set station name");
    Config::setRegistrationState(Config::RegistrationState::STATION_NAME);
  }
  else {
    return logRegistrationFail("Unable to set station name", result);
  }

  return true;
}

bool ApiClass::doStationAddress(RegistrationResult* result) {
  bool actionResult = true;
  actionResult = publishAddress(
    Config::getAddressCountry().c_str(), 
    Config::getAddressCity().c_str(),
    Config::getAddressStreet().c_str(), 
    Config::getAddressNumber().c_str(),
    false
  );

  if (actionResult) {
    Logger::info("[ApiClass::registerStation()] Set station address");
    Config::setRegistrationState(Config::RegistrationState::STATION_ADDRESS);
  }
  else {
    return logRegistrationFail("Unable to set address", result);
  }
  return true;
}

bool ApiClass::doStationLocation(RegistrationResult* result) {
  int actionResult = publishLocation(
    Config::getLocationLatitude().toDouble(),
    Config::getLocationLongitude().toDouble(),
    false
  );

  if (actionResult) {
    Logger::info("[ApiClass::registerStation()] Set station location");
    Config::setRegistrationState(Config::RegistrationState::STATION_LOCATION);
  }
  else {
    return logRegistrationFail("Unable to set location", result);
  }
  return true;
}

bool ApiClass::doTempSensor(RegistrationResult* result) {
  if (registerSensor("temperature")) {
    Logger::info("[ApiClass::registerStation()] Registered remperature sensor");
    Config::setRegistrationState(Config::RegistrationState::TEMP_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register remperature sensor", result);
  }
  return true;
}

bool ApiClass::doHumiditySensor(RegistrationResult* result) {
  if (registerSensor("humidity")) {
    Logger::info("[ApiClass::registerStation()] Registered humidity sensor");
    Config::setRegistrationState(Config::RegistrationState::HUMIDITY_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register humidity sensor", result);
  }
  return true;
}

bool ApiClass::doPreasurreSensor(RegistrationResult* result) {
  if (registerSensor("pressure")) {
    Logger::info("[ApiClass::registerStation()] Registered pressure sensor");
    Config::setRegistrationState(Config::RegistrationState::PREASSURE_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register pressure sensor", result);
  }
  return true;
}

bool ApiClass::doPM1Sensor(RegistrationResult* result) {
  if (registerSensor("pm1")) {
    Logger::info("[ApiClass::registerStation()] Registered pm1 sensor");
    Config::setRegistrationState(Config::RegistrationState::PM1_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register pm1 sensor", result);
  }
  return true;
}

bool ApiClass::doPM2_5Sensor(RegistrationResult* result) {
  if (registerSensor("pm2_5")) {
    Logger::info("[ApiClass::registerStation()] Registered pm2_5 sensor");
    Config::setRegistrationState(Config::RegistrationState::PM2_5_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register pm2_5 sensor", result);
  }
  return true;
}

bool ApiClass::doPM10Sensor(RegistrationResult* result) {
  if (registerSensor("pm10")) {
    Logger::info("[ApiClass::registerStation()] Registered pm10 sensor");
    Config::setRegistrationState(Config::RegistrationState::PM10_SENSOR);
  }
  else {
    return logRegistrationFail("Unable to register pm10 sensor", result);
  }
  return true;
}

bool ApiClass::doBtMacStatistic(RegistrationResult* result) {
  if (addBtMacStatistic()) {
    Logger::info("[ApiClass::doBtMacStatistic()] Created Bluetooth MAC statistic");
    Config::setRegistrationState(Config::RegistrationState::MAC_STATISTIC);
  }
  else {
    return logRegistrationFail("Unable to create Bluetooth MAC statistic", result);
  }
  return true;
}

bool ApiClass::doBtMacValue(RegistrationResult* result) {
  if (addBtMacValue()) {
    Logger::info("[ApiClass::doBtMacValue()] Sent Bluetooth MAC");
    Config::setRegistrationState(Config::RegistrationState::MAC_STATISTIC);
  }
  else {
    return logRegistrationFail("Unable to send Bluetooth MAC", result);
  }
  return true;
}

bool ApiClass::isRegistered() {
  return Config::getRegistrationState() == Config::RegistrationState::REGISTERED_OK;
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

bool ApiClass::addBtMacStatistic() {
  String url = Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId()
    + "/statistics";

  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(3));
  statisticDoc["statisticPrivacyMode"] = "private";
  statisticDoc["type"] = "one_string_value";
  statisticDoc["id"] = "btMacAddress";
  String body = "";
  serializeJson(statisticDoc, body);
  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);
  logRequest("[ApiClass::addBtMacStatistic()] Create statisctic: ", response);

  if (response.code != 201) {
    Logger::debug(String("[ApiClass::addBtMacStatistic()] Create statisctic fail - error: " + response.code).c_str());
    return false;
  }
  return true;
}

bool ApiClass::addBtMacValue() {
  String url = Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId()
    + "/statistics"
    + "/btMacAddress/values";
  DynamicJsonDocument macDoc(JSON_OBJECT_SIZE(1));
  macDoc["value"] = Bluetooth::getMAC().c_str();
  String body = "";
  serializeJson(macDoc, body);
  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);
  logRequest("[ApiClass::addBtMacValue()] Send MAC: ", response);

  if (response.code != 201) {
    Logger::debug(String("[ApiClass::addBtMacValue()] Sending statisctic fail - error: " + response.code).c_str());
    return false;
  }
  return true;
}

void ApiClass::logRequest(const char* name, Http::Response& response) {
  String logText = name;
  logText += response.code;
  logText += " payload: ";
  logText += response.payload;
  Logger::debug(&logText);
}

bool ApiClass::logRegistrationFail(const char* message, RegistrationResult* result) {
  String logMessage = "[ApiClass::registerStation()] ";
  logMessage += message;
  Logger::debug(&logMessage);
  result->message = message;
  result->ok = false;
  return false;
}

ApiClass Api;
