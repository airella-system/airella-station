#include "maintenance/Statistics.h"
#include <algorithm>
#include <functional>

void StatisticsClass::setConnectionTypeObject(DataModel& dataModel) const {
  const char* typeName = Config::getInternetConnectionType() == Config::InternetConnectionType::WIFI ? "WIFI" : "GSM";
  dataModel.addStatisticValue(StatisticValueString("connectionType", typeName));
}

void StatisticsClass::setHeartbeatObject(DataModel& dataModel) const {
  dataModel.addStatisticValue(StatisticValueString("heartbeat", "HEARTBEAT"));
}

void StatisticsClass::setConnectionStateObject(DataModel& dataModel) const {
  const char* strValue = Internet::isOk() ? "OK" : "ERROR";
  dataModel.addStatisticValue(StatisticValueString("connectionState", strValue));
}

void StatisticsClass::setPowerObject(DataModel& model) const {
  PowerSensor* powerSensor = DeviceContainer.powerSensor;
  if(!powerSensor->isInit()) return;
  PowerInfo powerInfo = powerSensor->getPowerInfo();
  model.addStatisticValue(StatisticValueFloat("busVoltage", powerInfo.busVoltage));
  model.addStatisticValue(StatisticValueFloat("current", powerInfo.current));
  model.addStatisticValue(StatisticValueFloat("loadVoltage", powerInfo.loadVoltage));
  model.addStatisticValue(StatisticValueFloat("power", powerInfo.power));
  model.addStatisticValue(StatisticValueFloat("shounVoltage", powerInfo.shounVoltage));
}

void StatisticsClass::setHeaterObject(DataModel& model) const {
  Heater* heater = DeviceContainer.heater;
  if(!heater->isInit()) return;
  HeaterReport heaterReport = heater->getReport();
  const char* strValue = heaterReport.isOn ? "ON" : "OFF";
  model.addStatisticValue(StatisticValueFloat("heaterTemp", heaterReport.temperature));
  model.addStatisticValue(StatisticValueFloat("heaterHum", heaterReport.humidity));
  model.addStatisticValue(StatisticValueFloat("heaterDewPoint", heaterReport.dewPoint));
  model.addStatisticValue(StatisticValueFloat("heaterPower", heaterReport.currentPower / 255.0 * 100.0));
  model.addStatisticValue(StatisticValueString("heaterState", strValue));
}

void StatisticsClass::reportBootUp() const {
  sendStringStatistic("boot", "BOOT");
}

bool StatisticsClass::sendFloatStatistic(const char* statisticId, double value) const {
  DynamicJsonDocument data(JSON_OBJECT_SIZE(1));
  data["value"] = value;
  return sendStatistic(statisticId, data);
}

bool StatisticsClass::sendStringStatistic(const char* statisticId, const char* value) const {
  DynamicJsonDocument data(JSON_OBJECT_SIZE(1));
  data["value"] = value;
  return sendStatistic(statisticId, data);
}

bool StatisticsClass::sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc) const {
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

const String StatisticsClass::getUrl() const {
  return Config::getApiUrl() 
    + "/stations/" 
    + Config::getApiStationId() 
    + "/statistics";
}

float StatisticsClass::calcTemperature() const {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getTemperature(); };
  return calc(getData);
}

float StatisticsClass::calcHumidity() const {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getHumidity(); };
  return calc(getData);
}

float StatisticsClass::calcPressure() const {
  WeatherSensor* sensor = DeviceContainer.weatherSensor;
  auto getData = [&sensor]() { return sensor->getPressure(); };
  return calc(getData);
}

template<typename F>
float StatisticsClass::calc(F &&getData) const {
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

float StatisticsClass::abs(float a) const {
  if(a < 0) return -1 * a;
  return a;
}

StatisticsClass Statistics;
