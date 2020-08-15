#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iterator>

class BluetoothChunkReceiver {
public:
  BluetoothChunkReceiver() {};
  ~BluetoothChunkReceiver() {};

  unsigned long maxBufferSize = 10;
  bool activeTransaction = false;
  std::vector<std::string> buffer;

  bool startTransaction() {
    if(activeTransaction) return false;
    activeTransaction = true;
    return true;
  }

  void endTransaction() {
    activeTransaction = false;
    buffer.clear();
  }

  bool add(std::string data) {
    if(!activeTransaction) return false;

    if(buffer.size() < maxBufferSize) {
      buffer.push_back(data);
      return true;
    }
    return false;
  }

  std::string getReceivedData() {
    if(buffer.empty()) return "";

    std::string data = "";
    std::vector<std::string>::iterator it = buffer.begin();
    while (it != buffer.end()) {
      data += *it;
      ++it;
    }

    return data;
  }
};

//BluetoothChunkReceiver btcr;
// btcr.startTransaction();
// btcr.add("20123456789012345678");
// btcr.add("mleko");
// btcr.add("long");
// btcr.add("pies");
// ReceivedData receivedData = btcr.getReceivedData();
// std::cout << receivedData.action << "\n";
// std::cout << receivedData.data << "\n";
// char* crc = btcr.endTransaction();
// for(int i = 0; i < 10; i++) {
//   std::cout << std::hex << (int)crc[i];
// }
// std::cout << "\n";