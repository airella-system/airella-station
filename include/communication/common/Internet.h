#pragma once

#include <Arduino.h>
#include "communication/wifi/WiFiConn.h"
#include "communication/gsm/GsmConn.h"
#include "communication/common/Http.h"

class Internet
{

public:
    enum Type
    {
        WIFI = 0,
        GSM = 1
    };

    static void setType(Type type);
    static int start();
    static void stop();
    static Http::Response httpGet(String url);
    static Http::Response httpPost(String url, String json);
    static Http::Response httpPut(String url, String json);
    static void setHeader(String key, String value);

private:
    static Type type;
};