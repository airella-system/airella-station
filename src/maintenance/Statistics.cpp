#include "maintenance/Statistics.h"

void StatisticsClass::reportBootUp() {
  sendStringStatistic("boot", "BOOT");
}

void StatisticsClass::reportConnectionType() {
  const char* typeName = Config::getInternetConnectionType() == Config::InternetConnectionType::WIFI ? "WIFI" : "GSM";
  sendStringStatistic("connectionType", typeName);
}

void StatisticsClass::reportHeater() {
  HeaterReport heaterReport = DeviceContainer.heater->getReport();
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
  PowerInfo powerInfo = DeviceContainer.powerSensor->getPowerInfo();
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
        (String("[StatisticsClass::createStatistic()] Create statistic fail - error: ") + String(response.code))
            .c_str());
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

  String url =
      Config::getApiUrl() + "/stations/" + Config::getApiStationId() + "/statistics/" + statisticId + "/values";

  String body;
  serializeJson(statisticDoc, body);
  Http::Response response = Internet::httpPost(url, body);

  if (response.code != 200) {
    Logger::debug((String("[StatisticsClass::sendStatistic()] Sending statisctic fail - error: ") +
                   String(response.code) + String(" ") + String(response.payload))
                      .c_str());
    return false;
  }
  String message = "[StatisticsClass::sendStatistic()] Send statistic '";
  message += statisticId;
  message += "' successful";
  Logger::debug(message);
  return true;
}

String StatisticsClass::getUrl() const {
  return Config::getApiUrl() + "/stations/" + Config::getApiStationId() + "/statistics";
}

bool StatisticsClass::isEmpty() {
  return bufferSize == 0;
}

void StatisticsClass::push(const char* value) {
  // if(bufferSize >= MAX_BUFFER_SIZE) return;
  // todo
}

const char* StatisticsClass::pop() {
  // todo
  return "";
}

StatisticsClass Statistics;
