#include "device/Heater.h"

void Heater::init() {
    analogWriteFrequency(HEATER_ON, 1000);
    analogWrite(HEATER_ON,heater);

    OneWire oneWire(OW1);
    DallasTemperature DS18B20(&oneWire);

    DS18B20.begin();

    HWSerial0.print("Parasite power is: "); 
    if( DS18B20.isParasitePowerMode() ){ 
        HWSerial0.println("ON");
    }else{
        HWSerial0.println("OFF");
    }
    
    numberOfDevices = DS18B20.getDeviceCount();
    HWSerial0.print( "Device count: " );
    HWSerial0.println( numberOfDevices );

    DS18B20.requestTemperatures();

    // Loop through each device, print out address
    for(int i=0;i<numberOfDevices; i++){
        // Search the wire for address
        if( DS18B20.getAddress(devAddr[i], i) ){
        //devAddr[i] = tempDeviceAddress;
        HWSerial0.print("Found device ");
        HWSerial0.print(i, DEC);
        HWSerial0.print(" with address: " + GetAddressToString(devAddr[i]));
        HWSerial0.println();
        }else{
        HWSerial0.print("Found ghost device at ");
        HWSerial0.print(i, DEC);
        HWSerial0.print(" but could not detect address. Check power and cabling");
        }

        //Get resolution of DS18b20
        HWSerial0.print("Resolution: ");
        HWSerial0.print(DS18B20.getResolution( devAddr[i] ));
        HWSerial0.println();

        //Read temperature from DS18b20
        float tempC = DS18B20.getTempC( devAddr[i] );
        HWSerial0.print("Temp C: ");
        HWSerial0.println(tempC);
    }
}

//Loop measuring the temperature
void TempLoop()
{
  for(int i=0; i<numberOfDevices; i++){
    float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
    tempDev[i] = tempC; //Save the measured value to the array
  }
  DS18B20.setWaitForConversion(false); //No waiting for measurement
  DS18B20.requestTemperatures(); //Initiate the temperature measurement
}

void Heater::on() {
    Logger::info("Heater is ON");
    analogWrite(HEATER_ON, 10);
}

void Heater::off() {
    Logger::info("Heater is OFF");
    analogWrite(HEATER_ON, 0);
}

void Heater::run() {
    
}

void Heater::stop() {
    heater-=1;
    HWSerial0.printf("heater set to: %d \n",heater);
    analogWrite(HEATER_ON,heater);
}

float Heater::getTemperature() {

}

bool Heater::getHeaterState() {

}
