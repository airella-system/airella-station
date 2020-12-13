#pragma once

#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/DeviceContainer.h"
#include "maintenance/Logger.h"
#include "time/Time.h"

#define MAX_BUFFER_SIZE 30

struct StatisticEnumDefinition {
  String id;
  String name;
};

class StatisticsClass {
 public:
  void reportBootUp();
  void reportConnectionType();
  void reportHeater();
  void reportHeartbeat();
  void reportConnectionState();
  void reportPower();

  bool sendFloatStatistic(const char* statisticId, double value);
  bool sendStringStatistic(const char* statisticId, const char* value);
  bool sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc);

  JsonObject createMultipleFloatsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    const String& metric, 
    const String& chartType
  );
  JsonObject createMultipleEnumsStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode,
    StatisticEnumDefinition enumDefinitions[], 
    int enumDefinitionsNum,
    const String& chartType
  );
  JsonObject createStringStatisticObject(
    const String& id, 
    const String& name, 
    const String& privacyMode
  );

  float calcTemperature();
  float calcHumidity();
  float calcPressure();

 private:
  const unsigned int dataArraySize = 5;
  String getUrl() const;
  const char** buffer;
  unsigned char bufferSize = 0;
  bool isEmpty();
  template<typename F>
  float calc(F &&getData);
  float abs(float a);
};

extern StatisticsClass Statistics;
