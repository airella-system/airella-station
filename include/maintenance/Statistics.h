#pragma once

#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/DeviceContainer.h"
#include "maintenance/Logger.h"
#include "time/Time.h"
#include "api/entity/DataModel.h"

#define MAX_BUFFER_SIZE 30

struct StatisticEnumDefinition {
  String id;
  String name;
};

class StatisticsClass {
 public:
  const JsonObject getBootUpObject() const;
  const JsonObject getConnectionTypeObject() const;
  const JsonObject getHeartbeatObject() const;
  const JsonObject getConnectionStateObject() const;
  const JsonObject getStatisticObject(const char* type, const char* value) const;
  const JsonObject getStatisticObject(const char* type, const float value) const;
  void setPowerObject(DataModel& model) const;
  void setHeaterObject(DataModel& model) const;

  const JsonObject createMultipleFloatsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    const String& metric, 
    const String& chartType
  ) const;
  const JsonObject createMultipleEnumsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    StatisticEnumDefinition enumDefinitions[], 
    int enumDefinitionsNum,
    const String& chartType
  ) const;
  const JsonObject createStringStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode
  ) const;

  float calcTemperature() const;
  float calcHumidity() const;
  float calcPressure() const;

  void reportBootUp() const;
  bool sendFloatStatistic(const char* statisticId, double value) const;
  bool sendStringStatistic(const char* statisticId, const char* value) const;
  bool sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc) const;

 private:
  const unsigned int dataArraySize = 5;
  String getUrl() const;
  const char** buffer;
  unsigned char bufferSize = 0;
  bool isEmpty();
  template<typename F>
  float calc(F &&getData) const;
  float abs(float a) const;
};

extern StatisticsClass Statistics;
