#pragma once

#include <ArduinoJson.h>
#include "maintenance/Logger.h"
#include "maintenance/StatisticTypes.h"

class RegiserModel {
public:
  RegiserModel() : doc(2048) {
    JsonObject additionalData = doc.createNestedObject("additionalData");
    JsonObject add = additionalData.createNestedObject("add");
    sensors = add.createNestedArray("sensors");
    statistics = add.createNestedArray("statistics");
    statisticValues = add.createNestedArray("statisticValues");
    set = additionalData.createNestedObject("set");
    address = set.createNestedObject("address");
    location = set.createNestedObject("location");
  }

  DynamicJsonDocument doc;
  JsonArray sensors;
  JsonArray statistics;
  JsonObject set;
  JsonObject address;
  JsonObject location;
  JsonArray statisticValues;

  void addSensor(const char* type) {
    JsonObject sensor = sensors.createNestedObject();
    sensor["type"] = type;
    sensor["id"] = type;
  }

  void addSensor(const char* type, const char* id) {
    JsonObject sensor = sensors.createNestedObject();
    sensor["type"] = type;
    sensor["id"] = id;
  }

  void addStatistic(const StringStatistic& statistic) {
    JsonObject object = statistics.createNestedObject();
    object["id"] = statistic.id;
    object["name"] = statistic.name;
    object["privacyMode"] = statistic.privacyMode;
    object["type"] = statistic.type;
  }

  void addStatistic(const MultipleEnumsStatistic& statistic) {
    JsonObject object = statistics.createNestedObject();
    object["privacyMode"] = statistic.privacyMode;
    object["id"] = statistic.id;
    object["name"] = statistic.name;
    object["type"] = statistic.type;
    object["chartType"] = statistic.chartType;

    JsonArray enumDefs = object.createNestedArray("enumDefinitions");
    for (int i = 0; i < statistic.enumDefinitionsNum; i++) {
      JsonObject enumDef = enumDefs.createNestedObject();
      enumDef["id"] = statistic.enumDefinitions[i].id;
      enumDef["name"] = statistic.enumDefinitions[i].name;
    }
  }

  void addStatistic(const MultipleFloatsStatistic& statistic) {
    JsonObject object = statistics.createNestedObject();
    object["privacyMode"] = statistic.privacyMode;
    object["id"] = statistic.id;
    object["name"] = statistic.name;
    object["type"] = statistic.type;
    object["metric"] = statistic.metric;
    object["chartType"] = statistic.chartType;
  }

  void setName(const char* name) {
    set["name"] = name;
  }

  void setAddress(const char* country, const char* city, const char* street, const char* number) {
    address["country"] = country;
    address["city"] = city;
    address["street"] = street;
    address["number"] = number;
  }

  void setLocation(const double latitude, const double longitude) {
    location["latitude"] = latitude;
    location["longitude"] = longitude;
  }

  void setBTMAC(const char* mac) {
    JsonObject statistic;
    statistic["statisticId"] = "mac";
    JsonObject statisticValue = statistic.createNestedObject("statisticValue");
    statisticValue["value"] = mac;
    statisticValues.add(statisticValue);
  }

  void setRegisterToken(const char* token) {
    doc["stationRegistrationToken"] = token;
  }

};
