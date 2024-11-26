#include "at_handler.h"
#include "handlers.h"

#define SERIAL_AT Serial

static CAtHandler atHandler(&SERIAL_AT);
static GpioHandler gpio_handler(&atHandler);

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  atHandler.run();
}
