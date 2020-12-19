#pragma once

#include <ArduinoJson.h>
#include "maintenance/StatisticTypes.h"

class DataModel {
public:
  DataModel() : doc(2048) {
    add = doc.createNestedObject("add");
  }

  bool containsData = false;
  bool locationIsInit = false;
  bool measurementIsInit = false;
  bool statisticIsInit = false;
  DynamicJsonDocument doc;
  JsonObject add;
  JsonArray measurements;
  JsonArray statisticValues;
  JsonObject location;

  void addMeasurement(const char* type, float value) {
    if(!measurementIsInit) {
      measurements = add.createNestedArray("measurements");
      measurementIsInit = true;
      containsData = true;
    }
    JsonObject measurementObject;
    measurementObject["sensorId"] = type;
    JsonObject measurement = measurementObject.createNestedObject("measurement");
    measurement["value"] = value;
    measurements.add(measurement);
  }

  void initStatistics() {
    if(!statisticIsInit) {
      statisticValues = add.createNestedArray("statisticValues");
      statisticIsInit = true;
      containsData = true;
    }
  }

  void addStatisticValue(const StatisticValueString& statisticObject) {
    initStatistics();
    JsonObject object = statisticValues.createNestedObject();
    object["statisticId"] = statisticObject.type;
    object["value"] = statisticObject.value;
  }

  void addStatisticValue(const StatisticValueFloat& statisticObject) {
    initStatistics();
    JsonObject object = statisticValues.createNestedObject();
    object["statisticId"] = statisticObject.type;
    object["value"] = statisticObject.value;
  }

  void setLocation(const double latitude, const double longitude) {
    if(!locationIsInit) {
      JsonObject set = doc.createNestedObject("add");
      location = set.createNestedObject("location");
      locationIsInit = true;
      containsData = true;
    }
    location["latitude"] = latitude;
    location["longitude"] = longitude;
  }
};
