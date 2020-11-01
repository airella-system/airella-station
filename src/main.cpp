#include "core/Core.h"
#include "maintenance/Logger.h"

void setup() {
  core.setUp();
}

void loop() {
  Logger::info("[Core]: Start main");
  core.main();
  Logger::error("[Core]: Escape from main loop");
}
