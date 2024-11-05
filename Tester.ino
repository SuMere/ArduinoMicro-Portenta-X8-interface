#include "at_handler.h" 

#define SERIAL_AT Serial

static CAtHandler atHandler(&SERIAL_AT);

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  atHandler.run();
}
