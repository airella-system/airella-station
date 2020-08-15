#pragma once

#include <iterator>
#include <algorithm>
#include <cmath>
#include "communication/bluetooth/chunker/BluetoothChunker.h"

class BluetoothChunkSender : public BluetoothChunker {

public:
  BluetoothChunkSender(BLEService* pService, const char* cuuid, const uint32_t access);
  ~BluetoothChunkSender() {};
  // size in bytes, max header size is 8, because 8 * 8 = 64 bits = sizeof(unsigned long)
  unsigned char headerSize = 1;
  bool activeTransaction = false;
  std::string buffer;
  unsigned long totalChunkCount;
  unsigned long sentChunkCount;

  bool startTransaction(std::string& data) {
    if(activeTransaction) return false;
    if(std::log2(data.length()) > headerSize * 8) return false;
    activeTransaction = true;
    sentChunkCount = 0;
    buffer = data;
    totalChunkCount = (data.length() + headerSize) / BT_MTU;
    totalChunkCount += (data.length() + headerSize) % BT_MTU == 0 ? 0 : 1;
    return true;
  }

  void endTransaction() {
    activeTransaction = false;
  }

  std::string get() {
    if(totalChunkCount == sentChunkCount) {
      endTransaction();
      return "";
    }
    if(sentChunkCount == 0) {
      std::string chunk;

      for(int i = 0; i < headerSize; i++) {
        char numChunk = totalChunkCount >> (8 * i);
        chunk += " ";
        chunk[i] = chunk[i] & 00000000;
        chunk[i] = numChunk;
      }

      unsigned long bufferSize = buffer.size();
      int dataSize = std::min((unsigned long)(BT_MTU - headerSize), bufferSize);
      chunk += buffer.substr(0, dataSize);
      sentChunkCount++;
      return chunk;
    }

    unsigned long bufferSize = buffer.size();
    unsigned long startAt = sentChunkCount * BT_MTU - 1;
    unsigned long lenght = std::min((unsigned long)BT_MTU, bufferSize);
    sentChunkCount++;
    return buffer.substr(startAt, lenght);
  }

private:

};
