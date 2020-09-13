#include "maintenance/Statistics.h"

void StatisticsClass::reportBootUp() {
  sendStatistic("bootUp", timeProvider.getDataTime().toString().c_str());
}

void StatisticsClass::reportConnectioniType() {
  const char* typeName = 
    Config::getInternetConnectionType() == Config::InternetConnectionType::WIFI 
    ? "wifi" : "gsm";
  sendStatistic("connectionType", typeName);
}

void StatisticsClass::reportHeaterState(bool value) {
  const char* strValue = value ? "on" : "off";
  sendStatistic("heaterTemp", strValue);
}

void StatisticsClass::reportHeaterTemp(String& value) {
  sendStatistic("heaterTemp", value.c_str());
}

void StatisticsClass::reportHeartbeat() {
  sendStatistic("heartbeat", timeProvider.getDataTime().toString().c_str());
}

void StatisticsClass::reportConnectionState() {
  //todo: to decide
  //sendStatistic("connectionState", timeProvider.getDataTime().toString().c_str());
}

void StatisticsClass::reportPower() {
  PowerInfo powerInfo = DeviceContainer.powerSensor->getPowerInfo();
  String value;
  value += "bv:";
  value += powerInfo.busVoltage;
  value += ",c:";
  value += powerInfo.current;
  value += ",lv:";
  value += powerInfo.loadVoltage;
  value += ",p:";
  value += powerInfo.power;
  value += ",sv:";
  value += powerInfo.shounVoltage;
  sendStatistic("powerInfo", value.c_str());
}

void StatisticsClass::reportPm() {
  String value;
  value += "mp1:";
  value += DeviceContainer.airSensor->getPM1();
  value += ",mp2_5:";
  value += DeviceContainer.airSensor->getPM2_5();
  value += ",mp10:";
  value += DeviceContainer.airSensor->getPM10();
  sendStatistic("pmInfo", value.c_str());
}

void StatisticsClass::reportWeather() {
  String value;
  value += "h:";
  value += DeviceContainer.weatherSensor->getHumidity();
  value += ",p:";
  value += DeviceContainer.weatherSensor->getPressure();
  value += ",t:";
  value += DeviceContainer.weatherSensor->getTemperature();
  sendStatistic("weatcherInfo", value.c_str());
}

bool StatisticsClass::createStatistic(StatisticEntity& statistic) {
  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(3));
  statisticDoc["privacyMode"] = statistic.getPrivacyMode();
  statisticDoc["type"] = statistic.getStatisticType();
  statisticDoc["id"] = statistic.id;
  String body;
  serializeJson(statisticDoc, body);
  Http::Response response;

  response = Internet::httpGet(getUrl());
  if (response.code != 404) {
    Logger::debug(
      String("[StatisticsClass::createStatistic()] Already exists statistic: ") 
      + statistic.id
    );
    return false;
  }

  response = Internet::httpPost(getUrl(), body);
  if (response.code != 201) {
    Logger::debug(String("[StatisticsClass::createStatistic()] Create statistic fail - error: ") 
      + response.code + " , statistic " + statistic.id);
    return false;
  }
  Logger::debug(
    String("[StatisticsClass::createStatistic()] Create statistic new statistic: ") 
    + statistic.id
  );
  return true;
}

bool StatisticsClass::sendStatistic(const char* statisticId, const char* value) {
  String url = Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId()
    + "/statistics/"
    + statisticId
    + "/values";

  DynamicJsonDocument data(JSON_OBJECT_SIZE(1));
  data["value"] = value;
  String body;
  serializeJson(data, body);
  Http::Response response = Internet::httpPost(url, body);

  if (response.code != 201) {
    Logger::debug(String("[StatisticsClass::sendStatistic()] Sending statisctic fail - error: "
      + response.code).c_str());
    return false;
  }
  String message = "[StatisticsClass::sendStatistic()] Send statistic '";
  message += statisticId;
  message += "' successful";
  Logger::debug(message);
  return true;
}

String StatisticsClass::getUrl() const {
  return Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId()
    + "/statistics";
}

StatisticsClass Statistics;
