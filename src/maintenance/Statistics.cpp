#include "maintenance/Statistics.h"

StatisticsClass::StatisticsClass() {

}

void StatisticsClass::reportBootUp() {

}

bool StatisticsClass::createStatistic(StatisticEntity statistic) {
  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(3));
  statisticDoc["privacyMode"] = statistic.getPrivacyMode();
  statisticDoc["type"] = statistic.getStatisticType();
  statisticDoc["id"] = statistic.id;
  String body = "";
  serializeJson(statisticDoc, body);
  Http::Response response = Internet::httpPost(getUrl(), body, String("Bearer ") + accessToken);

  if (response.code != 201) {
    Logger::debug(String("[StatisticsClass::createStatistic()] Create statisctic fail - error: " + response.code).c_str());
    return false;
  }
  return true;
}

bool StatisticsClass::sendStatistic() {

}

String getUrl() {
  return Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId()
    + "/statistics";
}