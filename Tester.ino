#include "at_handler.h"
#include "handlers.h"

#define SERIAL_AT Serial

static CAtHandler atHandler(&SERIAL_AT);

GpioHandler gpio_handler(&atHandler);
CanHandler can_handler(&atHandler);
I2CHandler i2c_handler(&atHandler);
PwmHandler pwm_handler(&atHandler);
DacHandler dac_handler(&atHandler);
UartHandler uart_handler(&atHandler);

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  atHandler.run();
}
