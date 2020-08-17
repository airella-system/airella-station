#pragma once

#include "communication/bluetooth/chunker/BluetoothChunker.h"

class BluetoothChunker;

class BLEChunkerCallback : public BLECharacteristicCallbacks {

public:
  BLEChunkerCallback(BluetoothChunker* _chunker) {
    chunker = _chunker;
  }
  ~BLEChunkerCallback() {}
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic* pCharacteristic);

private: 
  BluetoothChunker* chunker;

};