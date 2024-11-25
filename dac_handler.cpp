#include "dac_handler.h"

#define GIGA_DAC A12

DacHandler::DacHandler() {}

DacHandler::~DacHandler() {}

int DacHandler::fromVoltageToDacCount(float voltage) {
    int count = static_cast<int>(voltage * 255 / 3.3);
    return count;
}

TesterError DacHandler::setOutputVoltage(float voltage) {
    if (voltage < 0 || voltage > 3.3) {
        return ERROR_INVALID_ARGUMENT;
    }
    
    int count = this->fromVoltageToDacCount(voltage);

    pinMode(GIGA_DAC, OUTPUT);
    analogWrite(GIGA_DAC, count);
    return NO_ERROR;
}