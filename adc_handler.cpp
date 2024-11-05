#include "adc_handler.h"

#define GIGA_DAC A12

AdcHandler::AdcHandler() {}

AdcHandler::~AdcHandler() {}

int AdcHandler::fromVoltageToDacCount(float voltage) {
    int count = static_cast<int>(voltage * 255 / 3.3);
    return count;
}

TesterError AdcHandler::setOutputVoltage(float voltage) {
    if (voltage < 0 || voltage > 3.3) {
        return ERROR_INVALID_ARGUMENT;
    }
    
    int count = this->fromVoltageToDacCount(voltage);

    analogWrite(GIGA_DAC, count);
    return NO_ERROR;
}