#pragma once

#include <Arduino.h>
#include "device/DeviceContainer.h"
#include "util/List.h"
#include "time/Time.h"
#include <ArduinoJson.h>
#include "api/entity/DataModel.h"

#define BUFFER_MAX_SIZE 2

class StorableBuffer {
public:
  StorableBuffer(const String& _name);
  ~StorableBuffer() {};
  void push(const DataModel& data);
  const String pop();
  bool isEmpty() const;
  bool needSync();
  void sync();
  static void createCatalogStructure(const String& _name);
private:
  bool initialized = false;
  String name;
  int lastFileNum;
  List storagedFiles;
  unsigned int bufferSize = 0;
  String buffer[BUFFER_MAX_SIZE];
  void saveToStorege();
  void loadFromStorege(List& list);
};
