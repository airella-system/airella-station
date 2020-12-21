#pragma once

#include <Arduino.h>

struct StatisticEnumDefinition {
  String id;
  String name;
};

struct StringStatistic {

  const char* privacyMode;
  const char* id;
  const char* name;
  const char* type = "ONE_STRING";

  StringStatistic(
    const char* _privacyMode,
    const char* _id,
    const char* _name
  ) :
  privacyMode(_privacyMode),
  id(_id),
  name(_name)
  {}
};

struct MultipleEnumsStatistic {

  const char* id;
  const char* name;
  const char* privacyMode;
  StatisticEnumDefinition* enumDefinitions;
  int enumDefinitionsNum;
  const char* chartType;
  const char* type = "MULTIPLE_ENUMS";

  MultipleEnumsStatistic(
    const char* _id,
    const char* _name,
    const char* _privacyMode,
    StatisticEnumDefinition _enumDefinitions[],
    int _enumDefinitionsNum,
    const char* _chartType
  ) :
  id(_id),
  name(_name),
  privacyMode(_privacyMode),
  enumDefinitionsNum(_enumDefinitionsNum),
  chartType(_chartType)
  {
    enumDefinitions = _enumDefinitions;
  }
};

struct MultipleFloatsStatistic {

  const char* id;
  const char* name;
  const char* privacyMode;
  const char* metric;
  const char* chartType;
  const char* type = "MULTIPLE_FLOATS";

  MultipleFloatsStatistic(
    const char* _id,
    const char* _name,
    const char* _privacyMode,
    const char* _metric,
    const char* _chartType
  ) :
  id(_id),
  name(_name),
  privacyMode(_privacyMode),
  metric(_metric),
  chartType(_chartType)
  {}
};

struct StatisticValueString {

  const char* type;
  const char* value;

  StatisticValueString(const char* _type, const char* _value)
  : type(_type), value(_value) {}
};

struct StatisticValueFloat {

  const char* type;
  float value;

  StatisticValueFloat(const char* _type, float _value)
  : type(_type), value(_value) {}
};
