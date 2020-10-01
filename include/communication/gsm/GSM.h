#pragma once

#include "config/Config.h"
#include "maintenance/Logger.h"
#include "device/Sensor.h"
#include "communication/common/Http.h"

class GSM : public Sensor {
public:
  struct Response {
    bool success = true;
    int code = 0;
    String data;
  };
  GSM();
  ~GSM() {}
  Http::Response httpRequest(String& url, String& data);
  void powerOn();
  void powerOff();
private:
  GSMConfig config;
  HardwareSerial serial;
  void commandAsync(String& comand);
  void commandSync(String& comand, unsigned long timeout = 0);
  void commandSync(String& comand, const char* expectedResponse, unsigned long timeout = 0);
  GSM::Response commandSyncGetResponse(String& comand, unsigned long timeout = 0);
};