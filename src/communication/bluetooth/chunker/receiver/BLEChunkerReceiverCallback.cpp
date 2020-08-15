#include "communication/bluetooth/chunker/receiver/BLEChunkerReceiverCallback.h"

void BLEChunkerReceiverCallback::onRead(BLECharacteristic *pCharacteristic) {
  Logger::debug("[ChunkerUploadCallback::onRead()] called");
  chunker->endTransaction();
}

void BLEChunkerReceiverCallback::onWrite(BLECharacteristic* pCharacteristic) {
  Logger::debug("[BT Chunker] Received chunk");
  if(!chunker->isActiveTransaction()) {
    chunker->startTransaction();
  }
  
  std::string value = pCharacteristic->getValue();
  chunker->add(value);
}
