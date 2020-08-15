#pragma once

#include "communication/bluetooth/chunker/sender/BluetoothChunkSender.h"

class BluetoothChunkSender;

class BLEChunkerSenderCallback : public BLECharacteristicCallbacks {

public:
  BLEChunkerSenderCallback(BluetoothChunkSender* _chunker) {
    chunker = _chunker;
  }
  ~BLEChunkerSenderCallback() {}
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic* pCharacteristic);

private: 
  BluetoothChunkSender* chunker;

};