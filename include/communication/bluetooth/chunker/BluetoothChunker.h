#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iterator>
#include <BLEDevice.h>
#include "maintenance/Logger.h"

#define BT_CHUNKER_TIMEOUT 10 // timeout in sec
#define BT_MTU 20 // in bytes

class BluetoothChunker {

public:
	BluetoothChunker() {};
	~BluetoothChunker() {};

	bool activeTransaction = false;

	virtual bool startTransaction() = 0;
  virtual void endTransaction() = 0;
	virtual bool isActiveTransaction() = 0;

protected:
	BLECharacteristic* characteristic;

};
