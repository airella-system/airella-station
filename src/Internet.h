

#pragma once

#include <Arduino.h>
#include "WiFiConn.h"
#include "GsmConn.h"

class Internet
{

public:
    enum Type
    {
        WIFI = 0,
        GSM = 1
    };

    static void setType(Type type);
    static void start();
    static void stop();
    static void httpGet(String url);
    static void httpPost(String url, String json);
    static void httpPut(String url, String json);

private:
    static Type type;
};