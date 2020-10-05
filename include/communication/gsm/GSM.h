#pragma once

#include "config/Config.h"
#include "maintenance/Logger.h"
#include "device/Sensor.h"
#include "communication/common/Http.h"
#include <iostream>
#include <regex>
#include <string>

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

  struct ProxyPayload {

  };

  GSM();
  ~GSM() {}
  Http::Response httpGetRequest(String& url);
  Http::Response httpPostRequest(String& url, String& data);
  void powerOn();
  void powerOff();
private:
  GSMConfig config;
  HardwareSerial serial;
  void commandAsync(String& comand);
  void commandSync(String& comand, const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  void commandSync(const char* comand, const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  Response listenForData(unsigned long timeout = DEFAULT_TIMEOUT);
  bool waitForResponse(const char* expectedResponse, unsigned long timeout = DEFAULT_TIMEOUT);
  Response listenForBytes(unsigned long size, unsigned long timeout = DEFAULT_TIMEOUT);
  ParsedRequestInfo parseRequestInfo(String& data);
  unsigned long calculateInterval(unsigned long timestamp);
  Response readRequestData(unsigned long dataSize, unsigned long timeout = DEFAULT_TIMEOUT);
  bool sendData(String& data, unsigned long timeout = DEFAULT_TIMEOUT);
};