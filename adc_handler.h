#pragma once

#include "common.h"

class AdcHandler {
    public:
        AdcHandler();
        ~AdcHandler();
        TesterError setOutputVoltage(float voltage);

    private:
        int fromVoltageToDacCount(float voltage);
};
