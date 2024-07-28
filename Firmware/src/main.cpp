#include "mrf_hardware.hpp"

using namespace mrf;

void setup() {
  Serial.begin(9600);

  delay(500);

  MrfHardware::instance().begin();
}

long oldPosition  = -999;

void loop() {
    delay(1000);

    MrfHardware::instance().loop();
}
