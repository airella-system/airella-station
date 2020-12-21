#include "util/DataPersister.h"
#include "device/DeviceContainer.h"

DataPersister::DataPersister() {
  DeviceContainer.storage->tryToInit();
  if(!DeviceContainer.storage->isInit()) {
    Logger::debug("[DataPersister::DataPersister()] Storage is not initialized");
    return;
  }
  storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/measurements")) {
    storage->mkdir("/measurements");
  }
  if(!storage->exists("/logs")) {
    storage->mkdir("/logs");
  }
  initialized = true;
  lastBufferPersist = millis();
}

void DataPersister::flushBuffer(BufferType type) {
  if(abs(millis() - lastBufferPersist) > 1000 * 60 * 10) {
    Logger::debug("[DataPersister::flushBuffer()] Buffer is persisted, with timeout.");
    persistBuffer(type);
  }
}

void DataPersister::saveMeasurement(const char* sensor, float data) {
  if(!initialized) return;
  Logger::debug("[DataPersister::saveMeasurement()] Save new measurement data.");
  
  if(bufferCount == DATA_BUFFER_SIZE) {
    persistBuffer(BufferType::measurementBuferType);
  }
  buffer[bufferCount++] = 
    timeProvider.getDataTime().toString() 
    + ": [" + sensor + "]" + String(data, DEC) + "\n";
}

void DataPersister::saveLog(String& logMessage) {
  if(!initialized) return;
  if(bufferCount == DATA_BUFFER_SIZE) {
    persistBuffer(BufferType::logBuffer);
  }
  buffer[bufferCount++] = (logMessage + "\n").c_str();
}

void DataPersister::persistBuffer(BufferType type) {
  if(bufferCount == 0) return;
  FS* fs = DeviceContainer.storage->getStorage(); 
  String path;
  lastBufferPersist = millis();
  if(type == BufferType::measurementBuferType) {
    path = "/measurements/" + timeProvider.getDate().toString() + ".txt";
  }
  else {
    path = "/logs/" + timeProvider.getDate().toString() + ".txt";
  }
  
  File file = fs->open(path, FILE_APPEND);
  if(!file) {
    if(BufferType::logBuffer != type) {
      Logger::debug("[DataPersister::persistBuffer()] Unable to persist buffer.");
    }
    bufferCount = 0;
    return;
  }
  for(int i = 0; i < bufferCount; i++) {
    file.print(buffer[i]);
  }
  bufferCount = 0;
  if(BufferType::logBuffer != type) {
    Logger::debug("[DataPersister::persistBuffer()] Buffer is persisted.");
  }
  file.close();
}