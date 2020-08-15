#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iterator>
#include <BLEDevice.h>
#include "maintenance/Logger.h"
#include "communication/bluetooth/chunker/ChunkerReceiverCallback.h"
#include "communication/bluetooth/chunker/BLEChunkerReceiverCallback.h"

#define BT_CHUNKER_TIMEOUT 10 // timeout in sec

class BluetoothChunkReceiver {

public:
  BluetoothChunkReceiver(BLEService* pService, const char* cuuid, const uint32_t access);
  ~BluetoothChunkReceiver();

  bool startTransaction();
  std::string endTransaction();
  bool isActiveTransaction();
  bool add(std::string data);
  void setPermission(esp_gatt_perm_t perm);
  void setValue(std::string value);
  std::string getValue();
  void setCallback(ChunkerReceiverCallback*_callback);

private:
  BLECharacteristic* characteristic;
  unsigned long maxBufferSize = 10;
  bool activeTransaction = false;
  std::vector<std::string> buffer;
  std::string value;
  ChunkerReceiverCallback* callback;
  unsigned long timestamp = 0;

  void runCallback();
  std::string buildData();

};
