#pragma once

#include "communication/bluetooth/chunker/BluetoothChunker.h"
#include "communication/bluetooth/chunker/receiver/ChunkerReceiverCallback.h"
#include "communication/bluetooth/chunker/receiver/BLEChunkerReceiverCallback.h"

class BluetoothChunkReceiver : public BluetoothChunker {

public:
  BluetoothChunkReceiver(BLEService* pService, const char* cuuid, const uint32_t access);
  ~BluetoothChunkReceiver();

  bool startTransaction();
  void endTransaction();
	bool isActiveTransaction();

  bool add(std::string data);
  void setPermission(esp_gatt_perm_t perm);
  void setValue(std::string value);
  std::string getValue();
  void setCallback(ChunkerReceiverCallback* _callback);
  std::string buildData();

private:
  unsigned long maxBufferSize = 10;
  std::vector<std::string> buffer;
  std::string value;
  ChunkerReceiverCallback* callback;
  unsigned long timestamp = 0;

};
