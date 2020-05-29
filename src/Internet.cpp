#include "Internet.h"

Internet::Type Internet::type = Internet::WIFI;

void Internet::start()
{
    Internet::type == Internet::WIFI ? WiFiConn::start() : GsmConn::start();
}

void Internet::stop()
{
    Internet::type == Internet::WIFI ? WiFiConn::stop() : GsmConn::stop();
}

void Internet::httpGet(String url)
{
    Internet::type == Internet::WIFI ? WiFiConn::httpGet(url) : GsmConn::httpGet(url);
}

void Internet::httpPost(String url, String body)
{
    Internet::type == Internet::WIFI ? WiFiConn::httpPost(url, body) : GsmConn::httpPost(url, body);
}

void Internet::httpPut(String url, String body)
{
    Internet::type == Internet::WIFI ? WiFiConn::httpPut(url, body) : GsmConn::httpPut(url, body);
}

void Internet::setType(Type type)
{
    Internet::type = type;
}