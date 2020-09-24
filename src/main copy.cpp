// #include <HardwareSerial.h>
// #include <Printers.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
// #include <Adafruit_INA219.h>
// #include <Wire.h>
// #include <stdio.h>
// #include <analogWrite.h>
// #include <DallasTemperature.h>

// #include "FS.h"
// #include "SD.h"
// #include "SPI.h"
// #include "string.h"



// #define LED1  14
// #define SELGPS 12
// #define INA_SCL 27
// #define INA_SDA 4
// #define CFG0 32
// #define CFG1 35
// #define CFG2 34
// #define CFG3 39
// #define nG510_PWR 33
// #define HEATER_ON 25
// #define PMS7K_ON 26
// #define ADC_SUPP 36

// #define U1TX 13
// #define OW1 15

// #define U1RX 2
// #define U2RX 16
// #define U2TX 17
// #define SD_SS 5
// #define SD_CLK 18
// #define SD_MISO 19
// #define BME_SDA 21
// #define BME_SCL 22
// #define SD_MOSI 23

// #define U0RX 3
// #define U0TX 1

// #define ONE_WIRE_MAX_DEV 2 //overkill - in the basic version we have just one
// float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature

// HardwareSerial HWSerial0(0);
// HardwareSerial HWSerial1(1);
// HardwareSerial HWSerial2(2);

// int incomingByte = 0;
// int heater = 10;


// OneWire oneWire(OW1);
// DallasTemperature DS18B20(&oneWire);
// int numberOfDevices; //Number of temperature devices found
// DeviceAddress devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors

// //ina219 vars
// float shuntvoltage = 0;
// float busvoltage = 0;
// float current_mA = 0;
// float loadvoltage = 0;
// float power_mW = 0;


// Adafruit_INA219 ina(0x40);
// Adafruit_BME280 bme; // I2C
// TwoWire I2C_INA219 = TwoWire(0);
// TwoWire I2C_BME280 = TwoWire(1);


// void setup() {
//   // put your setup code here, to run once:
//   pinMode(LED1,OUTPUT);
//   pinMode(nG510_PWR,OUTPUT);
//   pinMode(SELGPS,OUTPUT);
//   pinMode(PMS7K_ON,OUTPUT);
//   digitalWrite(nG510_PWR,HIGH);
//   digitalWrite(SELGPS,LOW);
//   digitalWrite(PMS7K_ON,LOW);
  
//   analogWriteFrequency(HEATER_ON, 1000);
//   analogWrite(HEATER_ON,heater);
  
//   //void HardwareSerial::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms)
//   HWSerial0.begin(115200,SERIAL_8N1,U0RX,U0TX,false,1000);  //console
//   HWSerial1.begin(9600,SERIAL_8N1,U1RX,U1TX,false,1000);    //PMS7003
//   HWSerial2.begin(115200,SERIAL_8N1,U2RX,U2TX,false,1000);  //G510

//   HWSerial0.println("Ininitializing SD...");

//   if(!SD.begin()){
//         HWSerial0.println("Card Mount Failed");
//     }
//     else
//     {
//       HWSerial0.println("Card Mount OK");
//     }

//   delay(1000);
  
//   HWSerial0.println("initalizing sensors...");
//   //bool begin(int sda=-1, int scl=-1, uint32_t frequency=0);
//   if( I2C_BME280.begin(BME_SDA,BME_SCL,100000) == false ) HWSerial0.println("I2C_BME280.begin returned false"); else HWSerial0.println("I2C_BME280.begin returned true");
//   if( I2C_INA219.begin(INA_SDA,INA_SCL,100000) == false ) HWSerial0.println("I2C_INA219.begin returned false"); else HWSerial0.println("I2C_INA219.begin returned true");
  
//   unsigned status;
  
//   status = bme.begin(0x76, &I2C_BME280);  //try 0x77 or 0x76
//   if (!status) {
//       HWSerial0.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
//       HWSerial0.print("SensorID was: 0x"); HWSerial0.println(bme.sensorID(),16);
//       while (1) delay(10);
//   }
//   else
//   {
//     HWSerial0.println("bme.begin returned true");
//   }

//     ina.begin(&I2C_INA219);
  
// }


// int rxedAnything = 0;

// bool comandReady = false;
// int currentChar = 0;
// String cmd;
// String cmdChunk;

// void loop() {

//   while(!comandReady) {
//     if((HWSerial0.available() > 0)) {
//       if(currentChar == 13) {
//         comandReady = true;
//         cmd = cmdChunk;
//         cmdChunk = "";
//       }
//       else {
//         cmdChunk += currentChar;
//       }
//     }
//   }
  
//   if (HWSerial0.available() > 0) {
//     rxedAnything = 1;
//     incomingByte = HWSerial0.read();
//     HWSerial0.flush();
//     // HWSerial0.printf("Serial0 received: %c = %d \n",incomingByte,incomingByte);
    
//     if(incomingByte == 'G') {
//       HWSerial0.printf("G510 Power ON signal...");
//       digitalWrite(nG510_PWR,LOW);
//       delay(900); //min 800ms
//       digitalWrite(nG510_PWR,HIGH);
//       HWSerial0.printf("done\n");
//     }
//     if(incomingByte == 'g') {
//       HWSerial0.printf("G510 Power OFF signal...");
//       digitalWrite(nG510_PWR,LOW);
//       delay(3500); //min 3s
//       digitalWrite(nG510_PWR,HIGH);
//       HWSerial0.printf("done\n");
//     }
//     if(incomingByte == 'a') {
//       HWSerial0.printf("@at");
//       HWSerial2.printf("at\r\n");
//     }
//     if(incomingByte == 'c') {
//       HWSerial0.printf("@cmd");
//       HWSerial2.printf("AT+CGSN\r\n");
//     }
//     if(incomingByte == '1') {
//       HWSerial0.printf("@1");
//       HWSerial2.printf("AT+MIPCALL=1,\"internet\",\"internet\",\"internet\"\r\n");
//     }
//     if(incomingByte == '2') {
//       HWSerial0.printf("@2");
//       HWSerial2.printf("AT+HTTPSET=\"URL\",\"http://airella.cyfrogen.com/api/stations/1\"\r\n");
//     }
//     if(incomingByte == '3') {
//       HWSerial0.printf("@3");
//       HWSerial2.printf("AT+HTTPSET=\"UAGENT\",\"airella\"\r\n");
//     }
//     if(incomingByte == '4') {
//       HWSerial0.printf("@4");
//       HWSerial2.printf("AT+HTTPDATA=30\r\n");
//     }
//     if(incomingByte == '5') {
//       HWSerial0.printf("@5");
//       HWSerial2.printf("AT+HTTPACT=1,30\r\n");
//     }
//     if(incomingByte == '6') {
//       HWSerial0.printf("@6");
//       HWSerial2.printf("AT+HTTPREAD=0,30\r\n");
//     }
//     if(incomingByte == '7') {
//       HWSerial0.printf("@6");
//       HWSerial2.printf("AT+HTTPREAD=0,30\r\n");
//     }
//     if(incomingByte == '8') {
//       HWSerial0.printf("@8");
//       HWSerial2.printf("AT+HTTPREAD=0,50\r\n");
//     }
//   }

//   if (HWSerial2.available() > 0) {
//     rxedAnything = 1;
//     HWSerial0.printf("Data from HWSerial2: ");
//     while(HWSerial2.available() > 0)
//     {
//       incomingByte = HWSerial2.read();
//       HWSerial0.printf("%c",incomingByte);
//     }
//   }

//   digitalWrite(LED1, !digitalRead(LED1));


//   if(rxedAnything)
//   {
//     delay(1);
//     rxedAnything = 0;
//   }
//   else
//   {
//     delay(1000);
//   }
  
// }

// void sendSyncCmd() {

// }

// // void setup() {
// //   pinMode(LED1,OUTPUT);
// //   pinMode(nG510_PWR,OUTPUT);
// //   pinMode(SELGPS,OUTPUT);
// //   pinMode(PMS7K_ON,OUTPUT);
// //   digitalWrite(nG510_PWR,HIGH);
// //   digitalWrite(SELGPS,LOW);
// //   digitalWrite(PMS7K_ON,LOW);

// //   //Configure software serial
// //   HWSerial2.begin(115200,SERIAL_8N1,U2RX,U2TX,false,1000);
// //   //Configure Arduino serial
// //   HWSerial0.begin(115200,SERIAL_8N1,U0RX,U0TX,false,1000);
// //   //Indicate start:
// //   HWSerial0.println("-----");
// //   delay(5000);
// //   digitalWrite(nG510_PWR,LOW);
// //   delay(900); //min 800ms
// //   digitalWrite(nG510_PWR,HIGH);
// //   delay(5000);
// // }
 
// // void loop() {
// //   //Send AT command to G510; AT commands must finish with \r\n
// //   HWSerial2.println("AT");
// //   //get output form the chip
// //   while(HWSerial2.available()){
// //     HWSerial0.print( (char)HWSerial2.read() );
// //   }
// //   //Wait 1s
// //   delay(1000);
// // }