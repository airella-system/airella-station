#include "maintenance/Statistics.h"
#include <algorithm>
#include <functional>

void StatisticsClass::reportBootUp() {
  sendStringStatistic("boot", "BOOT");
}

void StatisticsClass::reportConnectionType() {
  const char* typeName = Config::getInternetConnectionType() == Config::InternetConnectionType::WIFI ? "WIFI" : "GSM";
  sendStringStatistic("connectionType", typeName);
}

void StatisticsClass::reportHeater() {
  Heater* heater = DeviceContainer.heater;
  if(!heater->isInit()) return;
  HeaterReport heaterReport = heater->getReport();
  sendFloatStatistic("heaterTemp", heaterReport.temperature);
  sendFloatStatistic("heaterHum", heaterReport.humidity);
  sendFloatStatistic("heaterDewPoint", heaterReport.dewPoint);
  sendFloatStatistic("heaterPower", heaterReport.currentPower);
  const char* strValue = heaterReport.isOn ? "ON" : "OFF";
  sendStringStatistic("heaterState", strValue);
}

void StatisticsClass::reportHeartbeat() {
  sendStringStatistic("heartbeat", "HEARTBEAT");
}

void StatisticsClass::reportConnectionState() {
  const char* strValue = Internet::isOk() ? "OK" : "ERROR";
  sendStringStatistic("connectionState", strValue);
}

void StatisticsClass::reportPower() {
  PowerSensor* powerSensor = DeviceContainer.powerSensor;
  if(!powerSensor->isInit()) return;
  PowerInfo powerInfo = powerSensor->getPowerInfo();
  sendFloatStatistic("busVoltage", powerInfo.busVoltage);
  sendFloatStatistic("current", powerInfo.current);
  sendFloatStatistic("loadVoltage", powerInfo.loadVoltage);
  sendFloatStatistic("power", powerInfo.power);
  sendFloatStatistic("shounVoltage", powerInfo.shounVoltage);
}

bool StatisticsClass::createMultipleFloatsStatistic(const String& id, const String& name, const String& privacyMode,
                                                    const String& metric, const String& chartType) {
  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(6) + 1024);
  statisticDoc["privacyMode"] = privacyMode;
  statisticDoc["id"] = id;
  statisticDoc["name"] = name;
  statisticDoc["type"] = "MULTIPLE_FLOATS";
  statisticDoc["metric"] = metric;
  statisticDoc["chartType"] = chartType;
  return createStatistic(statisticDoc);
}

bool StatisticsClass::createMultipleEnumsStatistic(const String& id, const String& name, const String& privacyMode,
                                                   StatisticEnumDefinition enumDefinitions[], int enumDefinitionsNum,
                                                   const String& chartType) {
  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(5) + enumDefinitionsNum * JSON_OBJECT_SIZE(2) +
                                   JSON_ARRAY_SIZE(enumDefinitionsNum) + 1024);
  statisticDoc["privacyMode"] = privacyMode;
  statisticDoc["id"] = id;
  statisticDoc["name"] = name;
  statisticDoc["type"] = "MULTIPLE_ENUMS";
  statisticDoc["chartType"] = chartType;

  JsonArray enumDefs = statisticDoc.createNestedArray("enumDefinitions");
  for (int i = 0; i < enumDefinitionsNum; i++) {
    JsonObject enumDef = enumDefs.createNestedObject();
    enumDef["id"] = enumDefinitions[i].id;
    enumDef["name"] = enumDefinitions[i].name;
  }
  return createStatistic(statisticDoc);
}

bool StatisticsClass::createStringStatistic(const String& id, const String& name, const String& privacyMode) {
  DynamicJsonDocument statisticDoc(JSON_OBJECT_SIZE(4) + 1024);
  statisticDoc["privacyMode"] = privacyMode;
  statisticDoc["id"] = id;
  statisticDoc["name"] = name;
  statisticDoc["type"] = "ONE_STRING";
  return createStatistic(statisticDoc);
}

bool StatisticsClass::createStatistic(DynamicJsonDocument statisticDoc) {
  String body;
  serializeJson(statisticDoc, body);
  Http::Response response;

  response = Internet::httpPost(getUrl(), body);
  if (response.code != 201) {
    Logger::debug(
      (String("[StatisticsClass::createStatistic()] Create statistic fail - error: ") 
      + String(response.code)).c_str()
    );
    Logger::debug(response.payload);
    return false;
  }
  Logger::debug(String("[StatisticsClass::createStatistic()] Created new statistic"));
  return true;
}

bool StatisticsClass::sendFloatStatistic(const char* statisticId, double value) {
  DynamicJsonDocument data(JSON_OBJECT_SIZE(1));
  data["value"] = value;
  return sendStatistic(statisticId, data);
}

bool StatisticsClass::sendStringStatistic(const char* statisticId, const char* value) {
  DynamicJsonDocument data(JSON_OBJECT_SIZE(1));
  data["value"] = value;
  return sendStatistic(statisticId, data);
}

bool StatisticsClass::sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc) {
  if (!Api.isAuth()) {
    Logger::debug("[Statistics::sendStatistic()] Authorization failed");
    return false;
  }

  String url = Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId() 
    + "/statistics/" 
    + statisticId 
    + "/values";

  String body;
  serializeJson(statisticDoc, body);
  Http::Response response = Internet::httpPost(url, body);

  if (response.code != 200) {
    Logger::debug(
      (String("[StatisticsClass::sendStatistic()] Sending statisctic fail - error: ") 
      + String(response.code) + String(" ") + String(response.payload)).c_str()
    );
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

bool StatisticsClass::isEmpty() {
  return bufferSize == 0;
}

float StatisticsClass::calcTemperature() {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getTemperature(); };
  return calc(getData);
}

float StatisticsClass::calcHumidity() {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getHumidity(); };
  return calc(getData);
}

float StatisticsClass::calcPressure() {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getPressure(); };
  return calc(getData);
}

template<typename F>
float StatisticsClass::calc(F &&getData) {
  float data[dataArraySize];
  for(unsigned int i = 0; i < dataArraySize; i++) {
    data[i] = getData();
    if(i != dataArraySize - 1) delay(25);
  }

  std::sort(data, data + dataArraySize);
  float dA = abs(data[0] - data[dataArraySize / 2]);
  float dB = abs(data[dataArraySize - 1] - data[dataArraySize / 2]);
  float delta = 0;
  if(dA > dB) delta = dB;
  else delta = dA;
  float sum = 0;
  unsigned int count = 0;
  float midValue = data[dataArraySize / 2];
  for(unsigned int i = 0; i < dataArraySize; i++) {
    if(abs(data[i] - midValue) <= delta) {
      sum += data[i];
      count++;
    }
  }
  return sum / count;
}

float StatisticsClass::abs(float a) {
  if(a < 0) return -1 * a;
  return a;
}

StatisticsClass Statistics;
