#include "communication/bluetooth/chunker/BluetoothChunker.h"

BluetoothChunker::BluetoothChunker(BLEService* pService, const char* cuuid, const uint32_t access) {
  characteristic = pService->createCharacteristic(cuuid, access);
  BLECallback = new BLEChunkerCallback(this);
  characteristic->setCallbacks(BLECallback);
};

BluetoothChunker::~BluetoothChunker() {
  delete BLECallback;
}

bool BluetoothChunker::startReceiving(std::string& headerChunk) {
  if (isActiveReceiving()) {
    Logger::error("[BluetoothChunker::startReceiving()] Reveiving transaction already started");
    return false;
  }

  if (headerChunk.length() < BT_CHUNKER_HEADER_SIZE) {
    Logger::error("[BluetoothChunker::startReceiving()] Invalid message");
    return false;
  }

  toReceiveChunkCount = 0;
  // copy num save in many chars to single long type variable
  for (int i = 0; i < BT_CHUNKER_HEADER_SIZE; i++) {
    // save sizeof(char) bytes
    toReceiveChunkCount = toReceiveChunkCount | headerChunk[i];
    // move all bytes of 8 byte in left, make space for next part of num
    if (i < BT_CHUNKER_HEADER_SIZE - 1) toReceiveChunkCount = toReceiveChunkCount << 8;
  }

  // remove the header from original chunk
  headerChunk = headerChunk.substr(BT_CHUNKER_HEADER_SIZE);
  activeReceiving = true;
  receiveTimestamp = millis();
  Logger::debug("[BluetoothChunker::startReceiving()] Started reveiving transaction");
  return true;
}

void BluetoothChunker::addChunk(std::string value) {
  if (!isActiveReceiving()) return;

  buffer.push_back(value);
  receiveChunkCount++;
  receiveTimestamp = millis();
  if (receiveChunkCount == toReceiveChunkCount) endReceiving();
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
  String message = "[BluetoothChunker::endReceiving()] Ended reveiving transaction: ";
  message += value.c_str();
  Logger::debug(message.c_str());

  callback->afterReceive();
}

bool BluetoothChunker::isActiveReceiving() {
  if (isReceiverTimeout()) clearReceivingTransaction();
  return activeReceiving;
}

std::string BluetoothChunker::startSending() {
  if (isActiveSending()) {
    Logger::error("[BluetoothChunker::startSending()] Sending transaction already started");
    return "";
  }

  callback->beforeSend();

  // if(std::log2(value.length()) > BT_CHUNKER_HEADER_SIZE * 8) {
  //   Logger::error("[BluetoothChunker::startSending()] To loong message");
  //   return "";
  // }
  activeSending = true;
  toSendChunkCount = (value.length() + BT_CHUNKER_HEADER_SIZE) / BT_MTU;
  toSendChunkCount += (value.length() + BT_CHUNKER_HEADER_SIZE) % BT_MTU == 0 ? 0 : 1;
  //todo: check it again
  if (toSendChunkCount > std::pow(2, BT_CHUNKER_HEADER_SIZE * 8)) {
    Logger::error("[BluetoothChunker::startSending()] To loong message");
    return "";
  }

  std::string firstChunk = "";

  if (sentChunkCount == 0) {
    for (int i = 0; i < BT_CHUNKER_HEADER_SIZE; i++) {
      char numChunk = toSendChunkCount >> (8 * i);
      firstChunk += " ";
      firstChunk[i] = firstChunk[i] & 00000000;
      firstChunk[i] = numChunk;
    }

    unsigned long bufferSize = value.size();
    int dataSize = std::min((unsigned long)(BT_MTU - BT_CHUNKER_HEADER_SIZE), bufferSize);
    firstChunk += value.substr(0, dataSize);
  }

  sentChunkCount++;
  sentTimestamp = millis();
  String message = "[BluetoothChunker::startSending()] Started sending transaction: ";
  message += value.c_str();
  Logger::debug(message.c_str());
  if (toSendChunkCount == sentChunkCount) endSending();
  return firstChunk;
}

void BluetoothChunker::endSending() {
  clearSendingTransaction();
  Logger::error("[BluetoothChunker::endSending()] Ended sending transaction");
}

bool BluetoothChunker::isActiveSending() {
  if (isSenderTimeout()) clearSendingTransaction();
  return activeSending;
}

void BluetoothChunker::setCallback(BluetoothChunkerCallback* _callback) {
  callback = _callback;
  callback->setChunker(this);
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
  if (toSendChunkCount == sentChunkCount) endSending();
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
