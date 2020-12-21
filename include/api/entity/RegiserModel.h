#pragma once

#include <ArduinoJson.h>
#include "maintenance/Logger.h"
#include "maintenance/StatisticTypes.h"

class RegiserModel {
public:
  RegiserModel() : doc(5000) {
    JsonObject additionalQuery = doc.createNestedObject("additionalQuery");
    JsonObject add = additionalQuery.createNestedObject("add");
    sensors = add.createNestedArray("sensors");
    statistics = add.createNestedArray("statistics");
    statisticValues = add.createNestedArray("statisticValues");
    set = additionalQuery.createNestedObject("set");
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

  void setName(const String& name) {
    set["name"] = name;
  }

  void setAddress(const String& country, const String& city, const String& street, const String& number) {
    address["country"] = country;
    address["city"] = city;
    address["street"] = street;
    address["number"] = number;
  }

  void setLocation(const double latitude, const double longitude) {
    location["latitude"] = latitude;
    location["longitude"] = longitude;
  }

  void setBTMAC(const String& mac) {
    JsonObject statisticObject = statisticValues.createNestedObject();
    statisticObject["statisticId"] = "btMacAddress";
    JsonObject statisticVelue = statisticObject.createNestedObject("statisticValue");
    statisticVelue["value"] = mac;
  }

  void setRegisterToken(const String& token) {
    Logger::debug(token);
    doc["stationRegistrationToken"] = token;
  }

};
