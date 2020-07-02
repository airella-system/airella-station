#include "device/Heater.h"

Heater::Heater() : communicationBus(config.oneWirePin) {
    Logger::info("[Heater] Initalizing ...");
    String message;
    analogWriteFrequency(config.analogPin, 1000);
    analogWrite(config.analogPin, 0);
    heaterStatus.heaterIsOn = false;
    heaterStatus.threadIsRunning = false;

    thermometer = DallasTemperature(&communicationBus);
    thermometer.begin();

    Logger::info("[Heater] Parasite power is: "); 
    if(thermometer.isParasitePowerMode()) { 
        Logger::info("[Heater] Parasite power is: ON");
    }
    else {
        Logger::info("[Heater] Parasite power is: OFF");
    }
    
    numberOfDevices = thermometer.getDeviceCount();
    Logger::info("[Heater] Number of device: " + numberOfDevices);

    thermometer.requestTemperatures();

    for(int i = 0; i < numberOfDevices; i++) {
        if(thermometer.getAddress(deviceAddress[i], i)) {
            message = "[Heater] Found device " + i;
            message += " with address: " + deviceAddressToString(deviceAddress[i]);
            Logger::info(&message);
        }
        else {
            message = "[Heater] Found ghost device at " + i;
            message += " but could not detect address. Check power and cabling";
            Logger::warning(&message);
        }

        Logger::info("[Heater] Resolution: " + thermometer.getResolution(deviceAddress[i]));
    }

    Logger::info("[Heater] initialized: OK");
}

Heater::~Heater() {
    if(heaterStatus.threadIsRunning) {
        stop();
    }
}

void Heater::on() {
    heaterStatus.heaterIsOn = true;
    analogWrite(config.analogPin, 10);
    Logger::info("Heater is ON");
}

void Heater::off() {
    heaterStatus.heaterIsOn = false;
    analogWrite(config.analogPin, 0);
    Logger::info("Heater is OFF");
}

void Heater::run() {
    heaterStatus.threadIsRunning = true;
    //xTaskCreatePinnedToCore(TaskFunction_t, Name, StackSize, void *pvParameters, Priority, TaskHandle_t, xCoreID)
    xTaskCreatePinnedToCore(threadFunction, "termoThread", 10000, this, 1, &termoThreadHandler, 1);
    Logger::info("[Heater] run heater thread");
}

void threadFunction(void * pvParameters) {
    Heater *heater = (Heater *) pvParameters;
    unsigned long lastTimestamp = millis();
    unsigned int interval = 10000;
    unsigned int delayInterval = 100;
    unsigned int resetArterFails = 5;
    unsigned int failsTolerantCount = interval / delayInterval * resetArterFails; // reset after overflow
    unsigned int possibleFails = failsTolerantCount;

    bool heaterIsOn = heater->getHeaterState().heaterIsOn;
    float lastTemperature = heater->getTemperature();
    float temperatureTrashold = 3;
    float temperatureLowerLimit = 0;

    while(true) {
        if(millis() - lastTimestamp > interval) {
            possibleFails = failsTolerantCount;
            lastTimestamp = millis();

            float currentTemperature = heater->getTemperature();
            if(!heaterIsOn && currentTemperature < temperatureLowerLimit && abs(currentTemperature - temperatureLowerLimit) > temperatureTrashold) {
                heater->on();
                heaterIsOn = true;
                heater->heaterStatus.heaterIsOn = heaterIsOn;
            }
            if(heaterIsOn && currentTemperature > temperatureLowerLimit && abs(currentTemperature - temperatureLowerLimit) > temperatureTrashold) {
                heater->off();
                heaterIsOn = false;
                heater->heaterStatus.heaterIsOn = heaterIsOn;
            }
        }
        else {
            possibleFails--;
            if(possibleFails < 1) {
                possibleFails = failsTolerantCount;
                lastTimestamp = millis();
            }
            delay(delayInterval);
        }
    }
}

void Heater::stop() {
    heaterStatus.threadIsRunning = false;
    vTaskDelete(&termoThreadHandler);
    Logger::info("[Heater] stopped heater thread");
}


float Heater::getTemperature() {
    thermometer.setWaitForConversion(false);
    thermometer.requestTemperatures();

    for(int i = 0; i < numberOfDevices; i++) {
        temperatureDevice[i] = thermometer.getTempC(deviceAddress[i]);
    }
    
    return temperatureDevice[0];
}

HeaterStatus Heater::getHeaterState() {
    return heaterStatus;
}
