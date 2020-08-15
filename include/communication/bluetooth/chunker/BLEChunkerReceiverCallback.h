#pragma once

#include "communication/bluetooth/chunker/BluetoothChunkReceiver.h"

class BluetoothChunkReceiver;

class BLEChunkerReceiverCallback : public BLECharacteristicCallbacks {

public:
  BLEChunkerReceiverCallback(BluetoothChunkReceiver* _chunker) {
    chunker = _chunker;
  }
  ~BLEChunkerReceiverCallback() {}
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic* pCharacteristic);

private: 
  BluetoothChunkReceiver* chunker;

};