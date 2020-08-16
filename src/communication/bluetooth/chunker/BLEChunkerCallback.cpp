#pragma once

#include "communication/bluetooth/chunker/BLEChunkerCallback.h"

void BLEChunkerCallback::onRead(BLECharacteristic *pCharacteristic) {
  Logger::debug("[BT Chunker] Sent chunk");
  if(!chunker->isActiveSending()) {
    chunker->setValue(chunker->getValue());
    std::string firstChunk = chunker->startSending();
    pCharacteristic->setValue(firstChunk);
    return;
  }
  
  pCharacteristic->setValue(chunker->getChunk());
}

void BLEChunkerCallback::onWrite(BLECharacteristic *pCharacteristic) {
  Logger::debug("[BT Chunker] Received chunk");
  std::string chunk = pCharacteristic->getValue();
  if(!chunker->isActiveReceiving()) {
    chunker->startReceiving(chunk);
  }
  chunker->addChunk(chunk);
}
