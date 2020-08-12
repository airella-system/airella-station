#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iterator>

#define CRC_SIZE 10 //CRC sum size in byte

struct ReceivedData {
  std::string data = "";
  unsigned long action = 0;
};

class BluetoothChunkReceiver {
public:
  BluetoothChunkReceiver() {};
  ~BluetoothChunkReceiver() {};

  // size in bytes, max header size is 8, because 8 * 8 = 64 bits = sizeof(unsigned long)
  unsigned char headerSize = 1;
  // max count item in buffer
  unsigned long maxBufferSize = 10;
  bool activeTransaction = false;
  std::vector<std::string> buffer;
  char crc[CRC_SIZE];

  bool startTransaction() {
    if(activeTransaction) return false;
    activeTransaction = true;
    return true;
  }

  char* endTransaction() {
    activeTransaction = false;
    countCRC();
    char* crcSum =  getCRC();
    buffer.clear();
    memset(crc, 0, sizeof(char) * CRC_SIZE);
    return crcSum;
  }

  //BT return value in std::string
  bool add(std::string data) {
    if(!activeTransaction) return false;

    if(buffer.size() < maxBufferSize) {
      buffer.push_back(data);
      return true;
    }
    return false;
  }

  ReceivedData getReceivedData() {
    ReceivedData reveivedData;
    if(buffer.empty()) return reveivedData;

    unsigned long action = 0;
    std::string headerChunk = buffer[0];
    
    //invalid message
    if(headerChunk.length() < headerSize) return reveivedData;

    //copy num save in many chars to single long type variable
    for(int i = 0; i < headerSize; i++) {
      // save sizeof(char) bytes
      action = action | headerChunk[i]; 
      // move all bytes of 8 byte in left, make space for next part of num
      if(i < headerSize - 1) action = action << 8;
    }

    std::string data = headerChunk.substr(headerSize);
    std::vector<std::string>::iterator it = buffer.begin();
    ++it;
    while (it != buffer.end()) {
      data += *it;
      ++it;
    }

    reveivedData.action = action;
    reveivedData.data = data;
    return reveivedData;
  }

  ///////////////////////////////////////
  //private

  char* getCRC() const {
    char* crcCopy = new char[CRC_SIZE];
    memcpy(crcCopy, crc, sizeof(char) * CRC_SIZE);
    return crcCopy;
  }

  void countCRC() {
    unsigned int index = 0;
    unsigned int weightIndex = 0;
    unsigned weight[] = {3, 5, 7};

    for(std::string chunk : buffer) {
      for(char singleChart : chunk) {
        crc[index] += singleChart * weight[weightIndex];
        index = (index + 1) % CRC_SIZE;
        weightIndex = (weightIndex + 1) % 4;
      }
    }
  }
  
  std::vector<std::string>* getBuffer() {
    return &buffer;
  }

  unsigned int getBufferSize() {
    return buffer.size();
  }

  void clearBuffer() {
    buffer.clear();
  }

  bool isEmpty() {
    return buffer.empty();
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