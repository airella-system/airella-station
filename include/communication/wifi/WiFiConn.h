#pragma once
#include "communication/common/Http.h"
#include <Arduino.h>

class WiFiConn
{

public:
    static int start();
    static void stop();
    static Http::Response httpGet(String url);
    static Http::Response httpPost(String url, String json);
    static Http::Response httpPut(String url, String json);
    static void setHeader(String key, String value);    
};