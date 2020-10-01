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
  Logger::info("[GSM::powerOn] Power ON: start");
  digitalWrite(config.powerPin, LOW);
  delay(900); //min 800ms
  digitalWrite(config.powerPin, HIGH);
  Logger::info("[GSM::powerOn] Power ON: end");
}

void GSM::powerOff() {
  Logger::info("[GSM::powerOff] Power OFF: start");
  digitalWrite(config.powerPin, LOW);
  delay(3500); //min 3s
  digitalWrite(config.powerPin, HIGH);
  Logger::info("[GSM::powerOff] Power ON: end");
}

Http::Response GSM::httpRequest(String& url, String& data) {

}

void GSM::commandAsync(String& comand) {
  Logger::debug("[GSM::commandAsync] Send AT command: " + comand);
  serial.print(comand + "\r\n");
}

void GSM::commandSync(String& comand, unsigned long timeout /* = 0 */) {
  Logger::debug("[GSM::commandSync] Send AT command: " + comand);
  serial.print(comand + "\r\n");

  unsigned long timestamp = millis();
  while(true) {
    if(timeout != 0 && abs(timestamp - millis()) > timeout) {
      Logger::debug("[GSM::commandSync] AT timeout");
      return;
    }

    while(serial.available() > 0) {
      if(serial.read() > -1) {
        Logger::debug("[GSM::commandSync] AT end");
        return;
      }
    }
  }
}

void GSM::commandSync(String& comand, const char* expectedResponse, unsigned long timeout /* = 0 */) {
  Logger::debug("[GSM::commandSync] Send AT command: " + comand);
  serial.print(comand + "\r\n");

  unsigned long timestamp = millis();
  String response;
  while(true) {
    if(timeout != 0 && abs(timestamp - millis()) > timeout) {
      Logger::debug("[GSM::commandSync] AT timeout");
      return;
    }

    while(serial.available() > 0) {
      response += serial.read();
      if(response == expectedResponse) {
        Logger::debug("[GSM::commandSync] Receive AT response: " + response);
        return;
      }
    }
  }
}

GSM::Response GSM::commandSyncGetResponse(String& comand, unsigned long timeout /* = 0 */) {
  Logger::debug("[GSM::commandSyncGetResponse] Send AT command: " + comand);
  serial.print(comand + "\r\n");

  GSM::Response response;
  unsigned long timestamp = millis();
  unsigned int receivedChar = 0;
  String receivedData;
  while(true) {
    if(timeout != 0 && abs(timestamp - millis()) > timeout) {
      Logger::debug("[GSM::commandSync] AT timeout");
      response.code = 1;
      response.success = false;
      return response;
    }

    while(serial.available() > 0) {
      receivedChar = serial.read();
      if(receivedChar == '\n') {
        Logger::debug("[GSM::commandSync] Receive AT response: " + receivedData);
        response.data = receivedData;
        return response;
      }
      receivedData += receivedChar;
    }
  }
}
