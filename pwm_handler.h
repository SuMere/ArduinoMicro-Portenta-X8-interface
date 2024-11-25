#pragma once

#include "common.h"

class PwmHandler {
    public:
        PwmHandler();
        ~PwmHandler();

        TesterError readPwmIn(int adcChannel, int *output);
        TesterError setPwmOut(int pwmChannel, int dutyCycle);
};