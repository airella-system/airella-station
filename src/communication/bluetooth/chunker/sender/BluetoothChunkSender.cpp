#include "communication/bluetooth/chunker/sender/BluetoothChunkSender.h"

BluetoothChunkSender::BluetoothChunkSender(BLEService* pService, const char* cuuid, const uint32_t access) {
  characteristic = pService->createCharacteristic(cuuid, access);
  characteristic->setCallbacks(new BLEChunkerSenderCallback(this));
};

bool BluetoothChunkSender::startTransaction() {
    if(activeTransaction) return false;
    if(std::log2(value.length()) > headerSize * 8) return false;
    activeTransaction = true;
    sentChunkCount = 0;
    totalChunkCount = (value.length() + headerSize) / BT_MTU;
    totalChunkCount += (value.length() + headerSize) % BT_MTU == 0 ? 0 : 1;
    return true;
}

void BluetoothChunkSender::endTransaction() {
    activeTransaction = false;
}

std::string BluetoothChunkSender::get() {
  if(totalChunkCount == sentChunkCount) {
    endTransaction();
    return "";
  }

  if(sentChunkCount == 0) {
    std::string chunk;

    for(int i = 0; i < headerSize; i++) {
    char numChunk = totalChunkCount >> (8 * i);
    chunk += " ";
    chunk[i] = chunk[i] & 00000000;
    chunk[i] = numChunk;
    }

    unsigned long bufferSize = value.size();
    int dataSize = std::min((unsigned long)(BT_MTU - headerSize), bufferSize);
    chunk += value.substr(0, dataSize);
    sentChunkCount++;
    return chunk;
  }

  unsigned long bufferSize = value.size();
  unsigned long startAt = sentChunkCount * BT_MTU - 1;
  unsigned long lenght = std::min((unsigned long)BT_MTU, bufferSize);
  sentChunkCount++;
  return value.substr(startAt, lenght);
}

bool BluetoothChunkSender::setValue(std::string value) {
  if(activeTransaction) {
    Logger::debug("[BluetoothChunkReceiver::setValue()] Unable to set value");
    return false;
  }
  characteristic->setValue(value);
  return true;
}

std::string BluetoothChunkSender::getValue() {
  return value;
}
