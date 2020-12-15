#include "util/StorableBuffer.h"

StorableBuffer::StorableBuffer(const String& _name) : name(_name) {
  DeviceContainer.storage->tryToInit();
  if(!DeviceContainer.storage->isInit()) {
    Logger::debug("[StorableBuffer::StorableBuffer()] Storage is not initialized");
    return;
  }
  createCatalogStructure(_name);
  FS* storage = DeviceContainer.storage->getStorage();
  File bufferDirectory = storage->open("/sbuffer/" + _name);

  while (true) {
    File entry =  bufferDirectory.openNextFile();
    if(!entry) break;
    String data = entry.name();
    storagedFiles.add(data);
    entry.close();
  }
  String* lastFileName = storagedFiles.getLast();
  if(lastFileName == nullptr) lastFileNum = 0;
  else {
    lastFileNum = atoi((*lastFileName).c_str());
  }
  initialized = true;
}

void StorableBuffer::createCatalogStructure(const String& _name) {
  FS* storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/sbuffer")) {
    storage->mkdir("/sbuffer");
  }
  if(!storage->exists("/sbuffer/" + _name)) {
    storage->mkdir("/sbuffer/" + _name);
  }
}

void StorableBuffer::push(const DataModel& data) {
  if(!initialized) return;
  if(bufferSize == BUFFER_MAX_SIZE) {
    saveToStorege();
    bufferSize = 0;
  }
  
  const String date = timeProvider.getDataTime().toISOString();
  if(data.measurementIsInit) {
    for(auto item : data.measurements) {
      item["measurement"]["date"] = date;
    }
  }
  if(data.statisticIsInit) {
    for(auto item : data.statisticValues) {
      item["statisticValue"]["date"] = date;
    }
  }
  String body;
  serializeJson(data.doc, body);
  buffer[bufferSize++] = body;
}

const String StorableBuffer::pop() {
  if(bufferSize == 0) return "";
  return buffer[--bufferSize];
}

bool StorableBuffer::isEmpty() const {
  return bufferSize == 0;
}

bool StorableBuffer::needSync() {
  return !isEmpty() || !storagedFiles.isEmpty();
}

void StorableBuffer::saveToStorege() {
  String data;
  for(const String item : buffer) {
    data += item + ";";
  }
  bufferSize = 0;
  ++lastFileNum;
  const String newFileName = String("/sbuffer/" + name + "/" + String(lastFileNum, DEC));
  DeviceContainer.storage->write(data.c_str(), newFileName.c_str());
  storagedFiles.add(newFileName);
}

void StorableBuffer::loadFromStorege(List& list) {
  String* fileName = storagedFiles.pop();
  if(fileName == NULL) return;
  String fileContent = DeviceContainer.storage->read((*fileName).c_str());
  
  unsigned int checkpoint = 0;
  unsigned int lastCheckpoint = 0;

  for(char singleChar : fileContent) {
    if(singleChar == ';') {
      list.add(String(fileContent.substring(lastCheckpoint, checkpoint)));
      lastCheckpoint = checkpoint + 1;
    }
    checkpoint++;
  }
  DeviceContainer.storage->remove((*fileName).c_str());
}

void StorableBuffer::sync() {
  if(!initialized) return;
  if(!Internet::isOk()) return;
  
  List list;
  List backupList;
  while (!isEmpty()) list.add(pop());
  
  loadFromStorege(list);
  while(!list.isEmpty()) {
    String* node = list.pop();

    if(!Api.publishDataModel(*node)) {
      backupList.add(*node);
    }
  }
  
  list = backupList;
  if(!list.isEmpty()) {
    String data; 
    while(!list.isEmpty()) {
      data += *list.pop() + "@";
    }
    ++lastFileNum;
    const String newFileName = String("/sbuffer/" + name + "/" + String(lastFileNum, DEC));
    DeviceContainer.storage->write(data.c_str(), newFileName.c_str());
    storagedFiles.add(newFileName);
  }
}
