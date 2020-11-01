#pragma once

#include <iostream>
#include <regex>
#include <string>
#include "communication/common/Http.h"
#include "config/Config.h"
#include "device/Sensor.h"
#include "maintenance/Logger.h"

#define DEFAULT_TIMEOUT 30 * 1000
#define SERIAL_BUFFER_SIZE 1024

class GSM : public Sensor {
 public:
  struct Response {
    bool success = true;
    int code = 0;
    String data;
  };

  struct ParsedRequestInfo {
    char status = 0;
    int httpCode = 0;
    unsigned long dataSize = 0;
    bool success = true;
  };

  GSM();
  ~GSM() {}
  Http::Response httpGetRequest();
  Http::Response httpPostRequest(String& data);
  void powerOn();
  void powerOff();
  bool isConnected();
  bool isOk();

 private:
  GSMConfig config;
  HardwareSerial serial;
  void commandAsync(const char* comand);
  void commandAsync(String& comand);
  bool commandSync(String& comand, const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  bool commandSync(const char* comand, const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  GSM::Response listenForData(unsigned long timeout = DEFAULT_TIMEOUT);
  bool waitForResponse(const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  GSM::Response listenForBytes(unsigned long size, unsigned long timeout = DEFAULT_TIMEOUT);
  ParsedRequestInfo parseRequestInfo(String& data);
  unsigned long calculateInterval(unsigned long timestamp);
  GSM::Response readRequestData(unsigned long dataSize, unsigned long timeout = DEFAULT_TIMEOUT);
  bool sendData(String& data, unsigned long timeout = DEFAULT_TIMEOUT);
};
