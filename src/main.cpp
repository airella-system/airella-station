#include "maintenance/Logger.h"
#include "core/Core.h"

void setup() {
  core.setUp();
}

void loop() {
  Logger::info("[Core]: Start main");
  core.main();
  Logger::error("[Core]: Escape from main loop");
}
