#pragma once

#include <ArduinoJson.h>
#include "api/Api.h"
#include "communication/common/Internet.h"
#include "config/Config.h"
#include "device/DeviceContainer.h"
#include "maintenance/Logger.h"
#include "time/Time.h"
#include "api/entity/DataModel.h"
#include "maintenance/StatisticTypes.h"

#define MAX_BUFFER_SIZE 30

class StatisticsClass {
 public:
  void setConnectionTypeObject(DataModel& dataModel) const;
  void setHeartbeatObject(DataModel& dataModel) const;
  void setConnectionStateObject(DataModel& dataModel) const;
  void setPowerObject(DataModel& model) const;
  void setHeaterObject(DataModel& model) const;

  float calcTemperature() const;
  float calcHumidity() const;
  float calcPressure() const;

  void reportBootUp() const;
  bool sendFloatStatistic(const char* statisticId, double value) const;
  bool sendStringStatistic(const char* statisticId, const char* value) const;
  bool sendStatistic(const char* statisticId, DynamicJsonDocument statisticDoc) const;

 private:
  const unsigned int dataArraySize = 5;
  const String getUrl() const;
  template<typename F>
  float calc(F &&getData) const;
  float abs(float a) const;
};

extern StatisticsClass Statistics;
