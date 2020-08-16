#include "communication/bluetooth/chunker/BluetoothChunker.h"

BluetoothChunker::BluetoothChunker(BLEService* pService, const char* cuuid, const uint32_t access) {
  characteristic = pService->createCharacteristic(cuuid, access);
  BLECallback = new BLEChunkerCallback(this);
  characteristic->setCallbacks(BLECallback);
};

BluetoothChunker::~BluetoothChunker() {
  delete characteristic;
  delete BLECallback;
}

bool BluetoothChunker::startReceiving(std::string& headerChunk) {
  if(isActiveReceiving()) return false;
  //invalid message
  if(headerChunk.length() < headerSize) return false;

  toReceiveChunkCount = 0;
  //copy num save in many chars to single long type variable
  for(int i = 0; i < headerSize; i++) {
    // save sizeof(char) bytes
    toReceiveChunkCount = toReceiveChunkCount | headerChunk[i]; 
    // move all bytes of 8 byte in left, make space for next part of num
    if(i < headerSize - 1) toReceiveChunkCount = toReceiveChunkCount << 8;
  }
  
  //remove header
  headerChunk = headerChunk.substr(headerSize);
  activeReceiving = true;
  receiveTimestamp = millis();
  return true;
}

void BluetoothChunker::addChunk(std::string value) {
  if(!isActiveReceiving()) return;

  buffer.push_back(value);
  receiveChunkCount++;
  receiveTimestamp = millis();
  if(receiveChunkCount == toReceiveChunkCount) endReceiving();
}

void BluetoothChunker::endReceiving() {
  std::string data = "";
  std::vector<std::string>::iterator it = buffer.begin();
  while (it != buffer.end()) {
    data += *it;
    ++it;
  }

  value = data;
  clearReceivingTransaction();
}

bool BluetoothChunker::isActiveReceiving() {
  if(isReceiverTimeout()) clearReceivingTransaction();
  return activeReceiving;
}

std::string BluetoothChunker::startSending() {
  if(isActiveSending()) return "";
  if(std::log2(value.length()) > headerSize * 8) return "";
  activeSending = true;
  toSendChunkCount = (value.length() + headerSize) / BT_MTU;
  toSendChunkCount += (value.length() + headerSize) % BT_MTU == 0 ? 0 : 1;

  std::string firstChunk = "";
  
  if(sentChunkCount == 0) {

    for(int i = 0; i < headerSize; i++) {
      char numChunk = toSendChunkCount >> (8 * i);
      firstChunk += " ";
      firstChunk[i] = firstChunk[i] & 00000000;
      firstChunk[i] = numChunk;
    }

    unsigned long bufferSize = value.size();
    int dataSize = std::min((unsigned long)(BT_MTU - headerSize), bufferSize);
    firstChunk += value.substr(0, dataSize);
  }

  sentChunkCount++;
  sentTimestamp = millis();
  if(toSendChunkCount == sentChunkCount) endSending();

  return firstChunk;
}

void BluetoothChunker::endSending() {
  clearSendingTransaction();
}

bool BluetoothChunker::isActiveSending() {
  if(isSenderTimeout()) clearSendingTransaction();
  return activeSending;
}

void BluetoothChunker::setCallback(BluetoothChunkerCallback* _callback) {
  callback = _callback;
  callback->setChunker(this);
}

void BluetoothChunker::setPermission(esp_gatt_perm_t perm) {
  characteristic->setAccessPermissions(perm);
}

bool BluetoothChunker::setValue(std::string _value) {
  value = _value;
}

std::string BluetoothChunker::getValue() {
  return value;
}

std::string BluetoothChunker::getChunk() {
  unsigned long bufferSize = value.size();
  unsigned long startAt = sentChunkCount * BT_MTU - 1;
  unsigned long lenght = std::min((unsigned long)BT_MTU, bufferSize);
  sentChunkCount++;
  if(toSendChunkCount == sentChunkCount) endSending();
  return value.substr(startAt, lenght);
}


bool BluetoothChunker::isReceiverTimeout() {
  return abs(millis() - receiveTimestamp) > BT_CHUNKER_TIMEOUT * 1000;
}

bool BluetoothChunker::isSenderTimeout() {
  return abs(millis() - sentTimestamp) > BT_CHUNKER_TIMEOUT * 1000;
}

void BluetoothChunker::clearReceivingTransaction() {
  activeReceiving = false;
  receiveChunkCount = 0;
  buffer.clear();
}

void BluetoothChunker::clearSendingTransaction() {
  activeSending = false;
  sentChunkCount = 0;
}
