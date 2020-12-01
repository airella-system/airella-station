#pragma once

#include <Arduino.h>
#include <FS.h>
#include "time/Time.h"

#define DATA_BUFFER_SIZE 10

class DataPersister {
public:
  enum BufferType {
    measurementBuferType,
    logBuffer
  };
  DataPersister();
  ~DataPersister() {};
  void saveMeasurement(String& sensor, String data);
  void saveLog(String& logMessage);
  void flushBuffer(BufferType type);

private:
  FS* storage;
  bool initialized = false;

  unsigned long lastBufferPersist = 0;
  String buffer[DATA_BUFFER_SIZE];
  int bufferCount = 0;

  void persistBuffer(BufferType type);
};
