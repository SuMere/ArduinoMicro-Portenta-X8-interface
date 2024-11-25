#pragma once

#include "common.h"

class DacHandler {
    public:
        DacHandler();
        ~DacHandler();
        TesterError setOutputVoltage(float voltage);

    private:
        int fromVoltageToDacCount(float voltage);
};
