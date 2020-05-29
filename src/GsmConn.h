#pragma once

#include <Arduino.h>

class GsmConn
{

public:
    static void start();
    static void stop();
    static void httpGet(String url);
    static void httpPost(String url, String json);
    static void httpPut(String url, String json);
};