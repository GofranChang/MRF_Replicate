#include "mrf_hardware.hpp"

using namespace mrf;

void setup() {
  Serial.begin(9600);

  MrfHardware::instance().begin();
}

long oldPosition  = -999;

void loop() {
    delay(10);
    MrfHardware::instance().set_film_counter();

    MrfHardware::instance().loop();
}
