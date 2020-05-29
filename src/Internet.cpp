#include "Internet.h"

Internet::Type Internet::type = Internet::WIFI;

int Internet::start()
{
    return Internet::type == Internet::WIFI ? WiFiConn::start() : GsmConn::start();
}

void Internet::stop()
{
    Internet::type == Internet::WIFI ? WiFiConn::stop() : GsmConn::stop();
}

Http::Response Internet::httpGet(String url)
{
    return Internet::type == Internet::WIFI ? WiFiConn::httpGet(url) : GsmConn::httpGet(url);
}

Http::Response Internet::httpPost(String url, String body)
{
    return Internet::type == Internet::WIFI ? WiFiConn::httpPost(url, body) : GsmConn::httpPost(url, body);
}

Http::Response Internet::httpPut(String url, String body)
{
    return Internet::type == Internet::WIFI ? WiFiConn::httpPut(url, body) : GsmConn::httpPut(url, body);
}

void Internet::setType(Type type)
{
    Internet::type = type;
}