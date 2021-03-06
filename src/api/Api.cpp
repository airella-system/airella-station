#include "api/Api.h"
#include "maintenance/Guardian.h"

RegistrationResult ApiClass::registerStation() {
  RegistrationResult result;
  result.ok = true;
  if (isRegistered()) {
    Logger::debug("[ApiClass::registerStation()] Station already registered");
    result.ok = false;
    result.message = "Station already registered";
    return result;
  }

  accessToken = String("");
  accessTokenMillis = 0;
  if (Config::getRegistratonToken().equals("")) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("[ApiClass::registerStation()] Registration fail - no registration token");
    result.ok = false;
    result.message = "No registration token";
    return result;
  }

  if (!Internet::isConnected()) {
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    Logger::debug("[ApiClass::registerStation()] Registration fail - no internet connection");
    result.ok = false;
    result.message = "No internet connection";
    return result;
  }

  RegiserModel registerModel;
  registerModel.setName(Config::getStationName());
  registerModel.setBTMAC(Bluetooth::getMAC());
  registerModel.setRegisterToken(Config::getRegistratonToken());
  registerModel.setAddress(
    Config::getAddressCountry(),
    Config::getAddressCity(),
    Config::getAddressStreet(),
    Config::getAddressNumber()
  );
  registerModel.setLocation(
    Config::getLocationLatitude().toDouble(), 
    Config::getLocationLongitude().toDouble()
  );
  
  registerModel.addSensor("temperature");
  registerModel.addSensor("humidity");
  registerModel.addSensor("pressure");
  registerModel.addSensor("pm1");
  registerModel.addSensor("pm2_5");
  registerModel.addSensor("pm10");

  StatisticEnumDefinition bootEnums[] = {{"BOOT", "Station boot"}};
  registerModel.addStatistic(
    MultipleEnumsStatistic("boot", "Boot", "PRIVATE", bootEnums, 1, "SCATTER")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("heaterTemp", "Heater temperature [°C]", "PRIVATE", "°C", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("heaterHum", "Relative humidity [% RH]", "PRIVATE", "%", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("heaterDewPoint", "Dew point temperature [°C]", "PRIVATE", "°C", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("heaterPower", "Applied PWM duty cycle to heater [%]", "PRIVATE", "%", "LINE")
  );
  StatisticEnumDefinition heaterStateEnums[] = {{"ON", "On"}, {"OFF", "Off"}};
  registerModel.addStatistic(
    MultipleEnumsStatistic("heaterState", "Heater status (On / Off)", "PRIVATE", heaterStateEnums, 2, "LINE")
  );
  StatisticEnumDefinition heartbeatEnums[] = {{"HEARTBEAT", "Heartbeat"}};
  registerModel.addStatistic(
    MultipleEnumsStatistic("heartbeat", "Heartbeat", "PRIVATE", heartbeatEnums, 1, "SCATTER")
  );
  StatisticEnumDefinition connectionTypeEnums[] = {{"WIFI", "WiFi"}, {"GSM", "GSM"}};
  registerModel.addStatistic(
    MultipleEnumsStatistic("connectionType", "Connection type", "PRIVATE", connectionTypeEnums, 2, "LINE")
  );
  StatisticEnumDefinition connectionStateEnums[] = {{"OK", "Connected"}, {"ERROR", "Error"}};
  registerModel.addStatistic(
    MultipleEnumsStatistic("connectionState", "Connection state", "PRIVATE", connectionStateEnums, 2, "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("busVoltage", "Air quality sensor bus voltage [V]", "PRIVATE", "V", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("shounVoltage", "Air quality sensor shunt voltage [mV]", "PRIVATE", "mV", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("loadVoltage", "Air quality sensor load voltage [V]", "PRIVATE", "V", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("current", "Air quality sensor current draw [mA]", "PRIVATE", "mA", "LINE")
  );
  registerModel.addStatistic(
    MultipleFloatsStatistic("power", "Air quality sensor power [mW]", "PRIVATE", "mW", "LINE")
  );
  registerModel.addStatistic(
    StringStatistic("PRIVATE", "btMacAddress", "Bluetooth MAC Address")
  );

  String url = Config::getApiUrl() + "/auth/register-station";

  String body;
  serializeJson(registerModel.doc, body);
  Http::Response response = Internet::httpPost(url, body);

  Logger::debug(
    String("[ApiClass::registerStation()] Registraton response code: ") 
    + response.code 
    + " payload: " 
    + response.payload
  );
  
  if (response.code != 201) {
    Logger::debug(String("[ApiClass::registerStation()] Registration fail - internet error: " + response.code).c_str());
    Config::setRegistrationState(Config::RegistrationState::REGISTRATION_ERROR);
    result.ok = false;
    result.message = "Unable to register";
    return result;
  }
  
  DynamicJsonDocument doc(1500);
  deserializeJson(doc, response.payload);
  const char* id = doc["data"]["id"];
  const char* refreshToken = doc["data"]["refreshToken"];
  const char* accessToken = doc["data"]["accessToken"]["token"];
  Config::setApiStationId(String(id));
  Config::setRefreshToken(String(refreshToken));
  Config::setAccessToken(String(accessToken));
  Config::setRegistrationState(Config::RegistrationState::REGISTERED);
  Logger::info("[ApiClass::registerStation()] Registered successfull");
  Config::setRegistrationState(Config::RegistrationState::REGISTERED);
  return result;
}

bool ApiClass::isRegistered() {
  return Config::getRegistrationState() == Config::RegistrationState::REGISTERED;
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
      const char* token = doc["data"]["accessToken"]["token"];
      accessToken = String(token);
      Config::setAccessToken(accessToken);
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

bool ApiClass::publishName(const char* name, bool authCheck /*= true*/) {
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

  String debugText = String("[ApiClass::publishName] Sensor set name response code: ") + response.code +
                     " payload: " + response.payload;
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

  String debugText = String("Sensor set location response code: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::publishAddress(const char* country, const char* city, const char* street, const char* number,
                              bool authCheck /*= true*/) {
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
  const size_t capacity = JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  doc["value"] = value;
  String body = "";
  serializeJson(doc, body);
  if(noInternetConnectionOptimalization()) return false;

  if (authCheck && !isAuth()) {
    Logger::debug("[ApiClass::publishMeasurement()] Authorization failed");
    return false;
  }

  String apiUrlBase = Config::getApiUrl();
  String url = apiUrlBase + "/stations/" + Config::getApiStationId() + "/sensors/" + sensor + "/measurements";

  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);

  String debugText = String("Add measurement response code: ") + response.code + " payload: " + response.payload;
  Logger::debug(debugText.c_str());

  return response.code == 200;
}

bool ApiClass::isAuth() {
  return isRegistered() && updateAccessToken();
}

bool ApiClass::addBtMacValue() {
  String url = Config::getApiUrl() + "/stations/" + Config::getApiStationId() + "/statistics" + "/btMacAddress/values";
  DynamicJsonDocument macDoc(JSON_OBJECT_SIZE(1));
  macDoc["value"] = Bluetooth::getMAC().c_str();
  String body = "";
  serializeJson(macDoc, body);
  Http::Response response = Internet::httpPost(url, body, String("Bearer ") + accessToken);
  logRequest("[ApiClass::addBtMacValue()] Send MAC: ", response);

  if (response.code != 200) {
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

bool ApiClass::noInternetConnectionOptimalization() {
  if(!Internet::isConnected()) {
    if(tryCount == 5 || abs(millis() - lastTryTime) > 1000 * 30)  {
      return false;
    }
    lastTryTime = millis();
    tryCount++;
    return true;
  } else {
    tryCount = 0;
  }
  return false;
}

bool ApiClass::publishDataModel(const String& body) {
  if(noInternetConnectionOptimalization()) return false;

  if (!isAuth()) {
    Logger::debug("[ApiClass::publishDataModel()] Authorization failed");
    return false;
  }

  Http::Response response = Internet::httpPost(
    Config::getApiUrl() + "/stations/" + Config::getApiStationId() + "/query", 
    body
  );

  Logger::debug(String("Add measurement response code: ") + response.code + " payload: " + response.payload);

  return response.code == 200;
}

ApiClass Api;
