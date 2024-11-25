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
#ifdef DAC_0 || DAC
    if (gpio == DAC_0 || gpio == DAC) {
        if(value == 1)
            analogWrite(gpio, 255);
        else{
            analogWrite(gpio, 0);
        }
        return opStatus;
    }
#endif

#ifdef DAC_1
    if (gpio == DAC_1) {
        if(value == 1)
            analogWrite(gpio, 255);
        else{
            analogWrite(gpio, 0);
        }
        return opStatus;
    }
#endif

    digitalWrite(gpio, value);

    return opStatus;
}
