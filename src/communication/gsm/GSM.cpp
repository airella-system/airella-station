#include "communication/gsm/GSM.h"

GSM::GSM() : serial(config.serialLine) {
  Logger::info("[GSM] Initalizing ...");
  pinMode(config.powerPin, OUTPUT);
  digitalWrite(config.powerPin, HIGH);
  serial.begin(115200, SERIAL_8N1, config.uartRx, config.uartTx, false, 1000);
  initialized = true;
  Logger::info("[GSM] Initalized: OK");
}

void GSM::powerOn() {
  powerOff();
  //commandAsync("AT+MRST");
  delay(5000);
  Logger::info("[GSM::powerOn] Power ON: start");
  digitalWrite(config.powerPin, LOW);
  delay(900); //min 800ms
  digitalWrite(config.powerPin, HIGH);
  delay(1000);
  Logger::info("[GSM::powerOn] Waiting for SIM card registration");
  serial.flush();
  if(waitForResponse("+SIM READY", 10 * 1000)) {
    Logger::info("[GSM::powerOn] SIM card registred");
  }
  else {
    Logger::info("[GSM::powerOn] Unable to register SIM card");
  }
  delay(10000);
  char tryCount = 2;
  while (
    !commandSync("AT+MIPCALL=1,\"internet\",\"internet\",\"internet\"", "OK", 10 * 1000)
    && tryCount > 0
  ) {
    tryCount--;
  }
  
  Logger::info("[GSM::powerOn] Power ON: end");
}

void GSM::powerOff() {
  Logger::info("[GSM::powerOff] Power OFF: start");
  digitalWrite(config.powerPin, LOW);
  delay(3500); //min 3s
  digitalWrite(config.powerPin, HIGH);
  Logger::info("[GSM::powerOff] Power OFF: end");
}

bool GSM::isConnected() {
  //todo
  return true;
}

bool GSM::isOk() {
  //todo
  return true;
}

Http::Response GSM::httpGetRequest() {
  Http::Response httpResponse;
  GSM::Response gsmResponse;
  commandSync("AT+HTTPSET=\"URL\",\"" + Config::getGsmExtenderUrl() + "\"", "OK");
  commandSync("AT+HTTPSET=\"UAGENT\",\"Airella\"", "OK");
  commandSync("AT+HTTPSET=\"CONTYPE\",\"application-json\"", "OK");

  if(!commandSync("AT+HTTPACT=0", "OK")) {
    return httpResponse;
  }

  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 20;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  if(gsmResponse.data == "+HTTPS: 0") {
    httpResponse.code = 21;
    httpResponse.payload = "Error connection";
    return httpResponse;
  }

  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 22;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  GSM::ParsedRequestInfo info = parseRequestInfo(gsmResponse.data);
  httpResponse.code = info.httpCode;
  
  gsmResponse = readRequestData(info.dataSize);
  if(!gsmResponse.success) {
    httpResponse.code = 23;
    httpResponse.payload = "Unable to get response data";
    return httpResponse;
  }
  httpResponse.payload = gsmResponse.data;

  serial.flush();
  return httpResponse;
}

Http::Response GSM::httpPostRequest(String& data) {
  Http::Response httpResponse;
  GSM::Response gsmResponse;
  commandSync("AT+HTTPSET=\"URL\",\"" + Config::getGsmExtenderUrl() + "\"", "OK");
  commandSync("AT+HTTPSET=\"UAGENT\",\"Airella\"", "OK");
  commandSync("AT+HTTPSET=\"CONTYPE\",\"application-json\"", "OK");

  if(sendData(data)) Logger::debug("[GSM::httpPostRequest] Send serial data ok");

  if(!commandSync("AT+HTTPACT=1", "OK")) {
    return httpResponse;
  }

  gsmResponse = listenForData();
  Logger::debug("mleko1");
  if(!gsmResponse.success) {
    httpResponse.code = 20;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  Logger::debug("mleko2");
  if(gsmResponse.data == "+HTTPS: 0") {
    httpResponse.code = 21;
    httpResponse.payload = "Error connection";
    return httpResponse;
  }
  Logger::debug("mleko3");
  gsmResponse = listenForData();
  Logger::debug("mleko4");
  if(!gsmResponse.success) {
    httpResponse.code = 22;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  Logger::debug("mleko5");
  GSM::ParsedRequestInfo info = parseRequestInfo(gsmResponse.data);
  httpResponse.code = info.httpCode;
  Logger::debug("mleko6");
  gsmResponse = readRequestData(info.dataSize);
  Logger::debug("mleko7");
  if(!gsmResponse.success) {
    httpResponse.code = 23;
    httpResponse.payload = "Unable to get response data";
    return httpResponse;
  }
  httpResponse.payload = gsmResponse.data;

  serial.flush();
  return httpResponse;
}

void GSM::commandAsync(const char* comand) {
  Logger::debug("[GSM::commandAsync] Send AT command: " + String(comand));
  serial.print(String(comand) + "\r\n");
}

void GSM::commandAsync(String& comand) {
  commandAsync(comand.c_str());
}

bool GSM::commandSync(String& comand, const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  return commandSync(comand.c_str(), expectedResponse, timeout);
}

bool GSM::commandSync(const char* command, const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  serial.flush();
  Logger::debug("[GSM::commandSync] Send AT command: " + String(command));
  serial.print(String(command) + "\r\n");

  String response;
  char receivedChar;
  unsigned long toIgnoreChars = strlen(command) + 3;
  unsigned long timestamp = millis();
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::commandSync] AT timeout: " + response);
      return false;
    }

    while(serial.available() > 0) {
      receivedChar = serial.read();
      if(toIgnoreChars > 0) {
        --toIgnoreChars;
      }
      else {
        response += receivedChar;
      }
      if(response == expectedResponse) {
        Logger::debug("[GSM::commandSync] Receive AT response: " + response);
        return true;
      }
      if(response == "ERROR") {
        Logger::debug("[GSM::commandSync] Receive AT response FAIL");
        return false;
      }
    }
  }
}

GSM::Response GSM::listenForData(unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  GSM::Response response;
  char receivedChar = 0;
  String receivedData = "";
  unsigned long timestamp = millis();
  bool nextCommanReady = false;
  Logger::debug("before");
  while(true) {
    yield();
    
    // Logger::debug("=");
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      // Logger::debug("@");
      Logger::debug("[GSM::listenForData] AT timeout: " + receivedData);
      response.code = 1;
      response.success = false;
      return response;
    }
    // Logger::debug("*");
    while(serial.available() > 0) {
      // Logger::debug("$");
      receivedChar = serial.read();
      // Logger::debug(String(receivedChar));
      if(receivedChar != '\n' && receivedChar != '\r') {
        receivedData += receivedChar;
        nextCommanReady = true;
      }
      if(nextCommanReady && receivedChar == '\n') {
        Logger::debug("[GSM::listenForData] Receive AT response: " + receivedData);
        response.data = receivedData;
        return response;
      }
    }
    delay(1);
    // Logger::debug("/");
  }
}

bool GSM::waitForResponse(const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  char receivedChar = 0;
  String receivedData;
  unsigned long timestamp = millis();
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::waitForResponse] AT timeout: " + receivedData);
      return false;
    }

    while(serial.available() > 0) {
      receivedChar = serial.read();
      if(receivedChar != '\n' && receivedChar != '\r') {
        receivedData += receivedChar;
      }
      if(receivedData == expectedResponse) {
        return true;
      }
    }
  }
}

GSM::Response GSM::listenForBytes(unsigned long size, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  GSM::Response response;
  char receivedChar = 0;
  char receivedData[size];
  unsigned long timestamp = millis();
  unsigned long readIndex = 0;
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::listenForBytes] AT timeout: " + String(receivedData));
      response.code = 1;
      response.success = false;
      return response;
    }
    Logger::serial.flush();
    while(serial.available() > 0) {
      receivedChar = serial.read();
      receivedData[readIndex++] = receivedChar;
      if(size == readIndex) {
        Logger::debug("[GSM::listenForBytes] Received all bytes.");
        response.data = String(receivedData);
        return response;
      }
    }
  }
}

GSM::ParsedRequestInfo GSM::parseRequestInfo(String& data) {
  GSM::ParsedRequestInfo result;
  if(data.length() == 0) return result;

  int numbers[3] = {0, 0, 0};
  int numberIndex = 0;
  int index = 0;
  for(int i = 0; i < data.length(); i++) {
    if(data[i] == '<') {
      index = i + 1;
      while(index < data.length() && data[index] != '>') {
        index++;
      }
      for(int numCharIndex = i + 1; numCharIndex < index; numCharIndex++) {
        numbers[numberIndex] *= 10;
        numbers[numberIndex] += data[numCharIndex] - 48;
      }
      numberIndex++;
    }
  }
  result.status = numbers[0];
  result.httpCode = numbers[1];
  result.dataSize = numbers[2];
  return result;
}

unsigned long GSM::calculateInterval(unsigned long timestamp) {
  unsigned long current = millis();
  if(timestamp <= current) return current - timestamp;
  else {
    unsigned long max = -1;
    return current - (timestamp - max);
  }
}

GSM::Response GSM::readRequestData(unsigned long dataSize, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  GSM::Response response;
  String command, expect;
  command = "AT+HTTPREAD=0,";
  command += dataSize;
  expect = "+HTTPREAD: ";
  expect += dataSize;
  expect += "\r\n";
  commandSync(command, expect.c_str());
  response = listenForBytes(dataSize);
  if(!response.success) return response;
  if(response.data.length() < 4) return response;

  int indexStart = 0;
  for(int i = 0; i < response.data.length() - 3; i++) {
    if(response.data[i] == '{') {
      indexStart = i;
      break;
    }
  }

  int indexEnd = 0;
  for(int i = response.data.length() - 1; i >= 0; i--) {
    if(response.data[i] == '}') {
      indexEnd = i;
      break;
    }
  }

  char* data = new char[indexEnd - indexStart];
  int tmpIndex = 0;
  for(int i = indexStart; i <= indexEnd; i++) {
    data[tmpIndex++] = response.data[i];
  }
  response.data = String(data);

  return response;
}

bool GSM::sendData(String& data, unsigned long timeout /* = DEFAULT_TIMEOUT */ ) {
  unsigned int dataSize = data.length();
  commandSync("AT+HTTPDATA=" + String(dataSize), ">");
  for(char singleChar : data) {
    serial.print(singleChar);
  }
  return waitForResponse("OK");
}
