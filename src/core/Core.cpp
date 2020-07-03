#include "core/Core.h"

Core::Core() {
    // Logger::info("[Core]: Iniatlizing...");

    // airSensor = new AirSensor();
    // weatherSensor = new WeatherSensor();
    // powerSensor = new PowerSensor();
    // heater = new Heater();
    // storage = new Storage();

    // Logger::info("[Core]: Iniatlized OK");
}

#define U0RX 3
#define U0TX 1

#define U2RX 16
#define U2TX 17

#define nG510_PWR 33

HardwareSerial HWSerial0(0);
HardwareSerial HWSerial2(2);

char incomingByte;
char incomingByte2 = 0;

void Core::setUp() {
    pinMode(nG510_PWR,OUTPUT);
    digitalWrite(nG510_PWR,HIGH);

    HWSerial0.begin(115200, SERIAL_8N1, U0RX, U0TX, false, 1000);
    HWSerial2.begin(115200, SERIAL_8N1, U2RX, U2TX, false, 1000);
}

String command;

void Core::loop() {
    if(HWSerial0.available() > 0) {
        incomingByte = HWSerial0.read();
        if(incomingByte == '\n') {
            HWSerial0.print("\n");
            command += "\r\n";
            HWSerial2.printf(command.c_str());
            command = "";
        } 
        else if(incomingByte == '*') {
            HWSerial0.print("send AT\n");
            HWSerial2.printf("at\r\n");
        }
        else {
            HWSerial0.print(incomingByte);
            command += incomingByte;
        }
    }

    if (HWSerial2.available() > 0) {
        HWSerial0.printf("Data from HWSerial2: ");
        while(HWSerial2.available() > 0) {
            incomingByte2 = HWSerial2.read();
            HWSerial0.print(incomingByte2);
        }
        HWSerial0.print("\n");
        HWSerial0.printf("\nend\n");
    }
}