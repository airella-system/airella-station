#include "maintenance/Statistics.h"

void StatisticsClass::reportBootUp() {

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
    Logger::debug(String("[StatisticsClass::createStatistic()] Already exists statistic: ") + statistic.id);
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
