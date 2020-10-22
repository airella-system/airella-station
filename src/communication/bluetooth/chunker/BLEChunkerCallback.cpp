#pragma once

#include "communication/bluetooth/chunker/BLEChunkerCallback.h"

void BLEChunkerCallback::onRead(BLECharacteristic *pCharacteristic) {
  if (!chunker->isActiveSending()) {
    chunker->setValue(chunker->getValue());
    std::string firstChunk = chunker->startSending();
    pCharacteristic->setValue(firstChunk);
    Logger::debug("[BT Chunker] Sent chunk");
    return;
  }

  Logger::debug("[BT Chunker] Sent chunk");
  pCharacteristic->setValue(chunker->getChunk());
}

void BLEChunkerCallback::onWrite(BLECharacteristic *pCharacteristic) {
  std::string chunk = pCharacteristic->getValue();
  if (!chunker->isActiveReceiving()) {
    chunker->startReceiving(chunk);
  }
  Logger::debug("[BT Chunker] Received chunk");
  chunker->addChunk(chunk);
}
