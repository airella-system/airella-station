#include "communication/gsm/GSM.h"
#include "communication/common/Http.h"

GSM::GSM() : serial(config.serialLine) {
  Logger::info("[GSM] Initalizing ...");
  pinMode(config.powerPin, OUTPUT);
  digitalWrite(config.powerPin, HIGH);
  serial.begin(115200, SERIAL_8N1, config.uartRx, config.uartTx, false, 1000);
  initialized = true;
  Logger::info("[GSM] Initalized: OK");
}

void GSM::powerOn() {
  delay(1000);
  Logger::info("[GSM::powerOn] Power ON: start");
  digitalWrite(config.powerPin, LOW);
  delay(900); //min 800ms
  digitalWrite(config.powerPin, HIGH);
  delay(1000);
  Logger::info("[GSM::powerOn] Waiting for SIM card registration");
  if(waitForResponse("+SIM READY", 30 * 1000)) {
    Logger::info("[GSM::powerOn] SIM card registred");
  }
  else {
    Logger::info("[GSM::powerOn] Unable to register SIM card");
  }
  commandSync("AT+MIPCALL=1,\"internet\",\"internet\",\"internet\"", "OK");
  Logger::info("[GSM::powerOn] Power ON: end");
}

void GSM::powerOff() {
  Logger::info("[GSM::powerOff] Power OFF: start");
  digitalWrite(config.powerPin, LOW);
  delay(3500); //min 3s
  digitalWrite(config.powerPin, HIGH);
  Logger::info("[GSM::powerOff] Power OFF: end");
}

Http::Response GSM::httpGetRequest(String& url) {
  Http::Response httpResponse;
  GSM::Response gsmResponse;
  commandSync("AT+HTTPSET=\"URL\",\"" + url + "\"", "OK");
  commandSync("AT+HTTPACT=0", "OK");
  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  if(gsmResponse.data == "+HTTPS: 0") {
    httpResponse.code = 400;
    httpResponse.payload = "Error connection";
    return httpResponse;
  }

  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  GSM::ParsedRequestInfo info = parseRequestInfo(gsmResponse.data);
  httpResponse.code = info.httpCode;
  
  /*gsmResponse = readRequestData(info.dataSize);
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = "Unable to get response data";
    return httpResponse;
  }
  httpResponse.payload = gsmResponse.data;*/

  serial.flush();
  return httpResponse;
}

Http::Response GSM::httpPostRequest(String& url, String& data) {
  Http::Response httpResponse;
  GSM::Response gsmResponse;
  commandSync("AT+HTTPSET=\"URL\",\"" + url + "\"", "OK");
  commandSync("AT+HTTPACT=1", "OK");
  sendData(data);
  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  if(gsmResponse.data == "+HTTPS: 0") {
    httpResponse.code = 400;
    httpResponse.payload = "Error connection";
    return httpResponse;
  }

  gsmResponse = listenForData();
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = gsmResponse.data;
    return httpResponse;
  }
  GSM::ParsedRequestInfo info = parseRequestInfo(gsmResponse.data);
  httpResponse.code = info.httpCode;
  
  /*gsmResponse = readRequestData(info.dataSize);
  if(!gsmResponse.success) {
    httpResponse.code = 400;
    httpResponse.payload = "Unable to get response data";
    return httpResponse;
  }
  httpResponse.payload = gsmResponse.data;*/

  serial.flush();
  return httpResponse;
}

void GSM::commandAsync(String& comand) {
  Logger::debug("[GSM::commandAsync] Send AT command: " + comand);
  serial.print(comand + "\r\n");
}

void GSM::commandSync(String& comand, const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  commandSync(comand.c_str(), expectedResponse, timeout);
}

void GSM::commandSync(const char* command, const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  serial.flush();
  Logger::debug("[GSM::commandSync] Send AT command: " + String(command));
  serial.print(String(command) + "\r\n");

  String response;
  char receivedChar;
  unsigned long toIgnoreChars = strlen(command) + 3;
  unsigned long timestamp = millis();
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::commandSync] AT timeout");
      Logger::debug(response);
      return;
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
        return;
      }
    }
  }
}

GSM::Response GSM::listenForData(unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  GSM::Response response;
  char receivedChar = 0;
  String receivedData;
  unsigned long timestamp = millis();
  bool nextCommanReady = false;
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::listenForData] AT timeout: " + receivedData);
      response.code = 1;
      response.success = false;
      return response;
    }
    
    while(serial.available() > 0) {
      receivedChar = serial.read();
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
  }
}

bool GSM::waitForResponse(const char* expectedResponse, unsigned long timeout /* = DEFAULT_TIMEOUT */) {
  char receivedChar = 0;
  String receivedData;
  unsigned long timestamp = millis();
  while(true) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::waitForResponse] AT timeout");
      Logger::debug(receivedData);
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

GSM::ParsedRequestInfo GSM::parseRequestInfo(String& _data) {
  GSM::ParsedRequestInfo result;
  std::string data = _data.c_str();
  std::regex regex("\\+HTTPRES: <([0-9]*)>,<([0-9]*)>,<([0-9]*)>.*");
  std::smatch matches;
  if(!std::regex_match(data, matches, regex)) {
    result.success = false;
    return result;
  }

  result.status = std::atoi(matches.str(1).c_str());
  result.httpCode = std::atoi(matches.str(2).c_str());

  //sdt::atoul() not working on arduino :/
  result.dataSize = 0;
  for(char number : matches.str(3)) {
    result.dataSize *= 10;
    result.dataSize += number - 48;
  }
  Logger::debug(String(result.dataSize));
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
  serial.setRxBufferSize(SERIAL_BUFFER_SIZE);
  serial.flush();
  GSM::Response response, gsmResponse;
  unsigned long timestamp = millis();
  unsigned long start;
  unsigned long end;
  int chunkIndex = 0;
  int allChunkCount = dataSize / SERIAL_BUFFER_SIZE;
  int lastChunkSize = dataSize % SERIAL_BUFFER_SIZE;
  String command, expect;

  while((chunkIndex + 1) <= allChunkCount) {
    if(timeout != 0 && calculateInterval(timestamp) > timeout) {
      Logger::debug("[GSM::readRequestData] AT timeout: " + String(response.data));
      response.code = 1;
      response.success = false;
      return response;
    }

    start = chunkIndex * SERIAL_BUFFER_SIZE;
    end = start + SERIAL_BUFFER_SIZE;
    command = "AT+HTTPREAD=";
    command += start;
    command += ",";
    command += end;
    expect = "+HTTPREAD: ";
    expect += end;
    expect += "\r\n";
    commandSync(command, expect.c_str());
    gsmResponse = listenForBytes(SERIAL_BUFFER_SIZE);
    response.data += gsmResponse.data;
    chunkIndex++;
  }

  if(lastChunkSize > 0) {
    start = chunkIndex * SERIAL_BUFFER_SIZE;
    end = start + lastChunkSize;
    command = "AT+HTTPREAD=";
    command += start;
    command += ",";
    command += end;
    expect = "+HTTPREAD: ";
    expect += end;
    expect += "\r\n";
    commandSync(command, expect.c_str());
    gsmResponse = listenForBytes(lastChunkSize);
    response.data += gsmResponse.data;
  }

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
