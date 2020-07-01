#include "core/Core.h"

Core::Core() {
    Logger::info("[Core]: Iniatlizing...");

    airSensor = new AirSensor();
    weatherSensor = new WeatherSensor();
    powerSensor = new PowerSensor();

    Logger::info("[Core]: Iniatlized OK");
}

void Core::setUp() {
    
}

void Core::loop() {

}