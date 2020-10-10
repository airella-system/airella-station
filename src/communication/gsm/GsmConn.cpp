#include "communication/gsm/GsmConn.h"

GSM GsmConn::gsm = GSM();

int GsmConn::start() {
  gsm.powerOn();
  return 0;
}

void GsmConn::stop() {
  gsm.powerOff();
}

bool GsmConn::isConnected() {
  return gsm.isConnected();
}

bool GsmConn::isOk() {
  return gsm.isOk();
}

Http::Response GsmConn::httpGet(const String& url, String& authorization) {
  return buildRequest(url, Http::Method::GET, authorization);
}

Http::Response GsmConn::httpPost(const String& url, String body, String& authorization) {
  return buildRequest(url, Http::Method::POST, authorization, &body);
}

Http::Response GsmConn::httpPut(const String& url, String body, String& authorization) {
  return buildRequest(url, Http::Method::PUT, authorization, &body);
}

Http::Response GsmConn::buildRequest(const String& url, Http::Method method, String& authorization, String* body /* = NULL */) {
  DynamicJsonDocument doc(2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 1024);
  switch (method) {
    case Http::Method::GET: {
      doc["method"] = "get";
    } break;
    case Http::Method::POST: {
      doc["method"] = "post";
    } break;
    case Http::Method::PUT: {
      doc["method"] = "put";
    } break;
  }
  doc["url"] = url;
  JsonObject headers = doc.createNestedObject("headers");
  headers["Authorization"] = authorization;
  String data;
  serializeJson(doc, data);
  if(body != NULL) {
    std::string editableData = data.c_str();
    data = editableData.substr(0, editableData.size() - 1).c_str();
    data += ",\"data\":" + *body + "}";
  }
  return gsm.httpPostRequest(data);
}
