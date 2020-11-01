#pragma once

#include "communication/bluetooth/chunker/BluetoothChunker.h"

class BluetoothChunker;

class BluetoothChunkerCallback {
 public:
  virtual void afterReceive() = 0;
  virtual void beforeSend() = 0;
  void setChunker(BluetoothChunker* _chunker) { chunker = _chunker; }

 protected:
  BluetoothChunker* chunker;
};
