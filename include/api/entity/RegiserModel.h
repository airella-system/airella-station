#pragma once

#include <ArduinoJson.h>

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
    JsonObject sensor;
    sensor["type"] = type;
    sensor["id"] = type;
    sensors.add(sensor);
  }

  void addStatistic(JsonObject statistic) {
    statistics.add(statistic);
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
