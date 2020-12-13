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
  sendFloatStatistic("heaterPower", heaterReport.currentPower / 255.0 * 100.0);
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

JsonObject StatisticsClass::createMultipleFloatsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    const String& metric, 
    const String& chartType
  ) {
  JsonObject object;
  object["privacyMode"] = privacyMode;
  object["id"] = id;
  object["name"] = name;
  object["type"] = "MULTIPLE_FLOATS";
  object["metric"] = metric;
  object["chartType"] = chartType;
  return object;
}

JsonObject StatisticsClass::createMultipleEnumsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    StatisticEnumDefinition enumDefinitions[], 
    int enumDefinitionsNum,
    const String& chartType
  ) {
  JsonObject object;
  object["privacyMode"] = privacyMode;
  object["id"] = id;
  object["name"] = name;
  object["type"] = "MULTIPLE_ENUMS";
  object["chartType"] = chartType;

  JsonArray enumDefs = object.createNestedArray("enumDefinitions");
  for (int i = 0; i < enumDefinitionsNum; i++) {
    JsonObject enumDef = enumDefs.createNestedObject();
    enumDef["id"] = enumDefinitions[i].id;
    enumDef["name"] = enumDefinitions[i].name;
  }
  return object;
}

JsonObject StatisticsClass::createStringStatisticObject(
  const String& id, 
  const String& name, 
  const String& privacyMode
  ) {
  JsonObject object;
  object["privacyMode"] = privacyMode;
  object["id"] = id;
  object["name"] = name;
  object["type"] = "ONE_STRING";
  return object;
}

JsonObject createStringStatisticObject(const String& id, const String& name, const String& privacyMode) {
  JsonObject object;
  object["id"] = id;
  object["name"] = name;
  object["privacyMode"] = privacyMode;
  return object;
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
