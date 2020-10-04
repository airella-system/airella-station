#include <HardwareSerial.h>
#include <Printers.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include <Wire.h>
#include <stdio.h>
#include <analogWrite.h>
#include <DallasTemperature.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "string.h"



#define LED1  14
#define SELGPS 12
#define INA_SCL 27
#define INA_SDA 4
#define CFG0 32
#define CFG1 35
#define CFG2 34
#define CFG3 39
#define nG510_PWR 33
#define HEATER_ON 25
#define PMS7K_ON 26
#define ADC_SUPP 36

#define U1TX 13
#define OW1 15

#define U1RX 2
#define U2RX 16
#define U2TX 17
#define SD_SS 5
#define SD_CLK 18
#define SD_MISO 19
#define BME_SDA 21
#define BME_SCL 22
#define SD_MOSI 23

#define U0RX 3
#define U0TX 1

#define ONE_WIRE_MAX_DEV 2 //overkill - in the basic version we have just one
float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature

HardwareSerial HWSerial0(0);
HardwareSerial HWSerial2(2);

int incomingByte = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1,OUTPUT);
  pinMode(nG510_PWR,OUTPUT);
  pinMode(SELGPS,OUTPUT);
  pinMode(PMS7K_ON,OUTPUT);
  digitalWrite(nG510_PWR,HIGH);
  digitalWrite(SELGPS,LOW);
  digitalWrite(PMS7K_ON,LOW);
  
  analogWriteFrequency(HEATER_ON, 1000);
  
  //void HardwareSerial::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms)
  HWSerial0.begin(115200,SERIAL_8N1,U0RX,U0TX,false,1000);  //console
  HWSerial2.begin(115200,SERIAL_8N1,U2RX,U2TX,false,1000);  //G510

}


int rxedAnything = 0;

bool comandReady = false;
int currentChar = 0;
String cmd;
String cmdChunk;
bool justNum = false;

void loop() {

  if((HWSerial0.available() > 0)) {
    currentChar = HWSerial0.read();
    if(currentChar == 13) {
      comandReady = true;
      cmd = cmdChunk;
      cmdChunk = "";
      HWSerial0.print("\n");
    }
    else {
      cmdChunk += (char)currentChar;
      HWSerial0.print(String((char)currentChar));
    }
  }
  
  if(comandReady) {
    HWSerial0.flush();
    if(cmd == "G") {
      HWSerial0.printf("G510 Power ON signal...");
      digitalWrite(nG510_PWR, LOW);
      delay(900); //min 800ms
      digitalWrite(nG510_PWR, HIGH);
      HWSerial0.printf("done\n");
    }
    else if(cmd == "g") {
      HWSerial0.printf("G510 Power OFF signal...");
      digitalWrite(nG510_PWR,LOW);
      delay(3500); //min 3s
      digitalWrite(nG510_PWR,HIGH);
      HWSerial0.printf("done\n");
    }
    else if(cmd == "a") {
      HWSerial0.printf("@at");
      HWSerial2.printf("at\r\n");
    }
    else if(cmd == "c") {
      HWSerial0.printf("@cmd");
      HWSerial2.printf("AT+CGSN\r\n");
    }
    else if(cmd == "1") {
      HWSerial0.printf("@1");
      HWSerial2.printf("AT+MIPCALL=1,\"internet\",\"internet\",\"internet\"\r\n");
    }
    else if(cmd == "2") {
      HWSerial0.printf("@2");
      HWSerial2.printf("AT+HTTPSET=\"URL\",\"http://airella.cyfrogen.com/api/stations\"\r\n");
    }
    else if(cmd == "3") {
      HWSerial0.printf("@5");
      HWSerial2.printf("AT+HTTPACT=0\r\n");
    }
    else if(cmd == "4") {
      HWSerial0.printf("@6");
      HWSerial2.printf("AT+HTTPREAD=0,100\r\n");
    }
    else if(cmd == "5") {
      HWSerial0.printf("@6");
      HWSerial2.printf("AT+HTTPREAD=100,200\r\n");
    }
    else if(cmd == "x") {
      HWSerial0.printf("@x");
      justNum = true;
    }
    else {
      if(justNum) {
        HWSerial0.printf((String("@read=") + cmd).c_str());
        HWSerial2.printf((String("AT+HTTPREAD=") + cmd + String("\r\n")).c_str());
      }
      else {
        HWSerial0.printf((String("@") + cmd).c_str());
        HWSerial2.printf((cmd + String("\r\n")).c_str());
      }
    }
    comandReady = false;
  }

  if (HWSerial2.available() > 0) {
    while(HWSerial2.available() > 0)
    {
      incomingByte = HWSerial2.read();
      HWSerial0.printf("%c",incomingByte);
    }
  } 
}


// #include "maintenance/Logger.h"
// #include "core/Core.h"

// void setup() {
//   core.setUp();
// }

// void loop() {
//   Logger::info("[Core]: Start main");
//   core.main();
//   Logger::error("[Core]: Escape from main loop");
// }
