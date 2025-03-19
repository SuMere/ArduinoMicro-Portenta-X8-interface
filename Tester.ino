#include "at_handler.h"
#include "handlers.h"

#define SERIAL_AT Serial

GpioHandler gpio_handler;
CanHandler can_handler;
I2CHandler i2c_handler;
PwmHandler pwm_handler;
DacHandler dac_handler;
UartHandler uart_handler;
Helper helper;

void setup() {
  SERIAL_AT.begin(115200);
  at_handler().begin(&SERIAL_AT);
  delay(1000);
}

void loop() {
  at_handler().run();
}
