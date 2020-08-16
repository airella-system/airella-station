#include "communication/bluetooth/chunker/sender/BLEChunkerSenderCallback.h"

void BLEChunkerSenderCallback::onRead(BLECharacteristic *pCharacteristic) {
  Logger::debug("[BT Chunker] Sent chunk");
  if(!chunker->isActiveTransaction()) {
    chunker->setValue(chunker->getValue());
    chunker->startTransaction();
  }
  
  pCharacteristic->setValue(chunker->get());
}

void BLEChunkerSenderCallback::onWrite(BLECharacteristic* pCharacteristic) {}
