#pragma once

#include <ArduinoJson.h>
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/DeviceContainer.h"
#include "maintenance/Logger.h"
#include "time/Time.h"
#include "api/Api.h"

#define MAX_BUFFER_SIZE 30


enum StatisticPrivacyMode { PUBLIC, PRIVATE };

enum StatisticGraphType{ SCATTER, LINE };

struct StatisticOneStringDefinition {
  String id;
  String name;
  StatisticPrivacyMode privacyMode;
};

struct StatisticMultipleFloatsDefinition {
  String id;
  String name;
  StatisticPrivacyMode privacyMode;
  String metric;
  StatisticGraphType chartType;
};

struct StatisticEnumDefinition {
  String id;
  String name;
};

struct StatisticMultipleEnumsDefinition {
  String id;
  String name;
  StatisticPrivacyMode privacyMode;
  StatisticEnumDefinition enumDefinitions[];
  int enumDefinitionsNum;
  StatisticGraphType chartType;
};




class StatisticsClass {
public:
//   StatisticOneStringDefinition BT_MAC_STATISTIC = {"", "", PUBLIC};

//   StatisticMultipleEnumsDefinition essa = {"", "", PUBLIC,  {{"BOOT", "Boot"}}, 1, LINE};




  void reportBootUp();
  void reportConnectionType();
  void reportHeater();
  void reportHeartbeat();
  void reportConnectionState();
  void reportPower();

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
