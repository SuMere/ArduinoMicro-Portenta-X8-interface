#include "gpio_handler.h"

#define GPIO_PATH "/sys/class/gpio"

GpioHandler::GpioHandler() {}

GpioHandler::~GpioHandler() {}

TesterError GpioHandler::getGpioStatus(int gpio, int *value) {
    TesterError opStatus = NO_ERROR;

    pinMode(gpio, INPUT);
    *value = digitalRead(gpio);

    return opStatus;
}

TesterError GpioHandler::setGpioStatus(int gpio, int value) {
    TesterError opStatus = NO_ERROR;

    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, value);

    return opStatus;
}
