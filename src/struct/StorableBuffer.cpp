#include "struct/StorableBuffer.h"

void StorableBuffer::push(String& data) {
  if(bufferSize < BUFFER_MAX_SIZE) {
    persist();
    bufferSize = 0;
  }
  buffer[bufferSize++] = data;
}

String StorableBuffer::pop() {
  if(bufferSize == 0) return "";
  return buffer[--bufferSize];
}

bool StorableBuffer::isEmpty() {
  return bufferSize == 0;
}

void StorableBuffer::saveToStorege() {
  String data = "";
  for(String item : buffer) {
    data += item + "\n";
  }
  bufferSize = 0;
  DeviceContainer.storage->append(data.c_str(), "buffer");
}

void StorableBuffer::loadFromStorege() {
  
}
