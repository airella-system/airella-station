#pragma once

template <typename T>
class BluetoothChunkCallback {

public:
  virtual void callback() = 0;
  void setChunker(T* _chunker) {
    chunker = _chunker;
  }

protected:
  T* chunker;
  
};
