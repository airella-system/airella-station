#pragma once

#include <ArduinoJson.h>
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

enum StatisticPrivacyMode { publicMode, privateMode };

enum StatisticType { ONE_STRING, MULTIPLE_FLOATS, MULTIPLE_ENUMS };

enum StatisticGraphMode { SCATTER, LINE };

class StatisticsClass {
 public:
  void reportBootUp();
  void reportConnectioniType();
  void reportHeaterState(bool value);
  void reportHeater(double temperature, double humidity, double dewPoint, double power, bool isOn);
  void reportHeartbeat();
  void reportConnectionState();
  void reportPower();
  void reportPm();
  void reportWeather();

  bool createMultipleFloatsStatistic(const String& id, const String& name, const String& privacyMode, const String& metric,
                                     const String& chartType);
  bool createMultipleEnumsStatistic(const String& id, const String& name, const String& privacyMode,
                                    StatisticEnumDefinition enumDefinitions[], int enumDefinitionsNum,
                                    const String& chartType);
  bool createStringStatistic(const String& id, const String& name, const String& privacyMode);
  bool createStatistic(DynamicJsonDocument statisticDoc);
  
  bool sendFloatStatistic(const char* statisticId, double value);
  bool sendStringStatistic(const char* statisticId, const char* value);
  bool sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc);

 private:
  String getUrl() const;
  const char** buffer;
  unsigned char bufferSize = 0;
  bool isEmpty();
  void push(const char* value);
  const char* pop();
};

extern StatisticsClass Statistics;
