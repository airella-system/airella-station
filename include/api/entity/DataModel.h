#pragma once

#include <ArduinoJson.h>

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
    }
    JsonObject measurementObject;
    measurementObject["sensorId"] = type;
    JsonObject measurement = measurementObject.createNestedObject("measurement");
    measurement["value"] = value;
    measurements.add(measurement);
    containsData = true;
  }

  void addStatisticValue(const JsonObject& statisticObject) {
    if(!statisticIsInit) {
      statisticValues = add.createNestedArray("statisticValues");
    }
    statisticValues.add(statisticObject);
    containsData = true;
  }

  void setLocation(const double latitude, const double longitude) {
    if(!locationIsInit) {
      JsonObject set = doc.createNestedObject("add");
      location = set.createNestedObject("location");
    }
    location["latitude"] = latitude;
    location["longitude"] = longitude;
    containsData = true;
  }
};
