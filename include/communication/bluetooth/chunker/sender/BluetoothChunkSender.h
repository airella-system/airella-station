#pragma once

#include <iterator>
#include <algorithm>
#include <cmath>
#include "communication/bluetooth/chunker/BluetoothChunker.h"
#include "communication/bluetooth/chunker/sender/BLEChunkerSenderCallback.h"

class BluetoothChunkSender : public BluetoothChunker {

public:
  BluetoothChunkSender(BLEService* pService, const char* cuuid, const uint32_t access);
  ~BluetoothChunkSender() {};
  
  bool startTransaction();
  void endTransaction();
  std::string get();
  bool setValue(std::string data);
  std::string getValue();

private:
  // size in bytes, max header size is 8, because 8 * 8 = 64 bits = sizeof(unsigned long)
  unsigned char headerSize = 1;
  std::string value;
  unsigned long totalChunkCount;
  unsigned long sentChunkCount;

};
