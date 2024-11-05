#pragma once

#include "common.h"

class PwmHandler {
    public:
        PwmHandler();
        ~PwmHandler();

        TesterError readPwmIn(int adcChannel, float *output);
};