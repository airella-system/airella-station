#pragma once

#include <Arduino.h>
#include "device/DeviceContainer.h"
#include "util/List.h"
#include "time/Time.h"

#define BUFFER_MAX_SIZE 5

struct BufferItem {
  String date;
  String data;
  String type;
};

class StorableBuffer {
public:
  StorableBuffer(String _name);
  ~StorableBuffer() {};
  void push(const char* type, const String& data);
  BufferItem pop();
  bool isEmpty();
  bool needSync();
  void sync();
private:
  String name;
  int lastFileNum;
  List storagedFiles;
  unsigned int bufferSize = 0;
  BufferItem buffer[BUFFER_MAX_SIZE];
  void saveToStorege();
  void loadFromStorege(MultiValueList& list);
};
