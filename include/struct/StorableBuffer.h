#pragma once

#include <Arduino.h>
#include "device/DeviceContainer.h"

#define BUFFER_MAX_SIZE 20

class StorableBuffer {
public:
  StorableBuffer();
  ~StorableBuffer();
  void push(String& data);
  String pop();
  bool isEmpty();

private:
  unsigned int bufferSize = 0;
  String buffer[BUFFER_MAX_SIZE];
  void saveToStorege();
  void loadFromStorege();
};
