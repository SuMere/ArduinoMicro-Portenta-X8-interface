#pragma once

#include "common.h"

class GpioHandler {
    public:
        GpioHandler();
        ~GpioHandler();

        TesterError getGpioStatus(int gpio, int *value);
        TesterError setGpioStatus(int gpio, int value);

};
