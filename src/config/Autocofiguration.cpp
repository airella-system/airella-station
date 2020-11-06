#include "config/Autocofiguration.h"

void Autocofiguration::tryConfig() {
  if(isConfigured() || !configFileExists() || !configFileIsCorect()) return;
  parseConfigFile();
  setConfig();
}

bool Autocofiguration::isConfigured() {

}

bool Autocofiguration::configFileExists() {

}

bool Autocofiguration::configFileIsCorect() {

}

void Autocofiguration::parseConfigFile(/*pointer here*/) {

}

void Autocofiguration::setConfig() {

}
