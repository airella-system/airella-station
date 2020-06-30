#include "device/Heater.h"

void Heater::init() {
    String message;
    analogWriteFrequency(HEATER_ON, 1000);
    analogWrite(HEATER_ON, 0);

    communicationBus = OneWire(OW1);
    thermometer = DallasTemperature(&communicationBus);

    thermometer.begin();

    Logger::info("Parasite power is: "); 
    if(thermometer.isParasitePowerMode()) { 
        Logger::info("Parasite power is: ON");
    }
    else {
        Logger::info("Parasite power is: OFF");
    }
    
    numberOfDevices = thermometer.getDeviceCount();
    Logger::info("Parasite power is: OFF" + numberOfDevices);

    thermometer.requestTemperatures();

    for(int i = 0; i < numberOfDevices; i++) {
        if(thermometer.getAddress(deviceAddress[i], i)) {
            message = "Found device " + i;
            message += " with address: " + deviceAddressToString(deviceAddress[i]);
            Logger::info(&message);
        }
        else {
            message = "Found ghost device at " + i;
            message += " but could not detect address. Check power and cabling";
            Logger::warning(&message);
        }

        Logger::info("Resolution: " + thermometer.getResolution(deviceAddress[i]));
    }

    Logger::info("[Heater] initialized.");
}

void Heater::on() {
    Logger::info("Heater is ON");
    heaterStatus.heaterIsOn = true;
    analogWrite(HEATER_ON, 10);
}

void Heater::off() {
    Logger::info("Heater is OFF");
    heaterStatus.heaterIsOn = false;
    analogWrite(HEATER_ON, 0);
}

void Heater::run() {
    heaterStatus.threadIsRunning = true;
    xTaskCreatePinnedToCore(thread, "termoThread", 10000, NULL, 1, &termoThread, 1);
}

void Heater::stop() {
    heaterStatus.threadIsRunning = false;
    vTaskDelete(&termoThread);
}

void Heater::thread(void * pvParameters) {
    
}

float Heater::getTemperature() {
    thermometer.setWaitForConversion(false);
    thermometer.requestTemperatures();

    for(int i=0; i<numberOfDevices; i++) {
        temperatureDevice[i] = thermometer.getTempC(deviceAddress[i]);
    }
    
    return temperatureDevice[0];
}

HeaterStatus Heater::getHeaterState() {
    return heaterStatus;
}
