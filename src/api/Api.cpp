#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "maintenance/Logger.h"
#include <ArduinoJson.h>


bool ApiClass::registerSensor(const char *type) {
  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::getApiStationId() + "/sensors";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["type"] = type;
  doc["id"] = type;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor add response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 201) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::updateAccessToken() {
  if (this->accessToken.equals("") ||
      (millis() - this->accessTokenMillis) > ACCESS_TOKEN_EXPIRATION_TIME) {
    String registrationToken = Config::getRegistratonToken();
    String apiUrlBase = Config::getApiUrl();
    String url = apiUrlBase + "/auth/refresh-token";
    String refreshToken = Config::getRefreshToken();

    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
    doc["refreshToken"] = refreshToken.c_str();
    String body = "";
    serializeJson(doc, body);

    Http::Response response = Internet::httpPost(url, body);

    String debugText = String("Get token response code: ") + response.code +
                       " payload: " + response.payload;
    Logger::debug(debugText.c_str());
    if (response.code == 200) {
      DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) +
                              280);
      deserializeJson(doc, response.payload);
      const char *token = doc["data"]["accessToken"]["token"];
      this->accessToken = String(token);
      this->accessTokenMillis = millis();
      return true;
    }
    return false;
  } else {
    return true;
  }
}

bool ApiClass::publishName(const char *name) {
  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::getApiStationId() + "/name";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
  doc["name"] = name;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor set name response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::publishLocation(double latitude, double longitude) {
  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::getApiStationId() + "/location";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::publishAddress(const char *country, const char *city,
                              const char *street, const char *number) {
  String apiUrlBase = Config::getApiUrl();
  String url = String(apiUrlBase) + "/stations/" +
               Config::getApiStationId() + "/address";

  DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
  doc["country"] = country;
  doc["city"] = city;
  doc["street"] = street;
  doc["number"] = number;
  String body = "";
  serializeJson(doc, body);

  Http::Response response = Internet::httpPut(url, body, String("Bearer ") + accessToken);

  String debugText = String("Sensor set location response code: ") +
                     response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code == 200) {
    return true;
  } else {
    return false;
  }
}

bool ApiClass::registerStation() {
  this->accessToken = String("");
  this->accessTokenMillis = 0;
  if (Config::getRegistratonToken().equals("")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("Registration fail - no registration token");
    return false;
  }

  if (!Internet::isConnected()) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("Registration fail - no internet connection");
    return false;
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

  String debugText = String("Registraton response code: ") + response.code +
                     " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  if (response.code != 201) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }

  DynamicJsonDocument doc2(2*JSON_OBJECT_SIZE(2) + 120);
  deserializeJson(doc2, response.payload);
  const char *id = doc2["data"]["id"];
  const char *refreshToken = doc2["data"]["refreshToken"];
  Config::setApiStationId(String(id));
  Config::setRefreshToken(String(refreshToken));

  if (!updateAccessToken()) {
      Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
      return false;
  }

  String name = Config::getStationName();
  if (!name.equals("")) {
    if(!publishName(name.c_str())) {
      Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
      return false;
    }
  }
  publishAddress(Config::getAddressCountry().c_str(),
      Config::getAddressCity().c_str(),
      Config::getAddressStreet().c_str(),
      Config::getAddressNumber().c_str());

  if (!registerSensor("temperature")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("humidity")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm1")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm2_5")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }
  if (!registerSensor("pm10")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    return false;
  }

  Config::setRegistrationState(Config::RegistrationState::REGISTERED);
  Config::save();
  return true;
}

bool ApiClass::isRegistered() {
  return Config::getRegistrationState() == Config::RegistrationState::REGISTERED;
}

bool ApiClass::publishMeasurement(String sensor, double value) {
  if (this->isRegistered() && this->updateAccessToken()) {
    String apiUrlBase = Config::getApiUrl();
    String url = apiUrlBase + "/stations/" +
                 Config::getApiStationId() + "/sensors/" + sensor +
                 "/measurements";

    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);
    Logger::debug(String(value).c_str());
    doc["value"] = value;
    String body = "";
    serializeJson(doc, body);

    Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);

    String debugText = String("Add measurement response code: ") +
                       response.code + " payload: " + response.payload;
    Logger::debug(debugText.c_str());

    if (response.code == 200) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void ApiClass::configUpdated() {
  Internet::stop();
  Internet::start();

  if (!this->isRegistered()) {
    this->registerStation();
    Config::save();
  } else {
    Logger::debug("Already registered");
  }
}

ApiClass Api;