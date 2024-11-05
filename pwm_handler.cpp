#include "pwm_handler.h"

#define PWM_SAMPLES 5
#define PWM_SAMPLE_TIMEOUT_US 1000000

PwmHandler::PwmHandler() {}

PwmHandler::~PwmHandler() {}

TesterError PwmHandler::readPwmIn(int adcChannel, float *output) {
    TesterError opStatus = NO_ERROR;
    float pulseLenght = 0;
    if(adcChannel < 0 || adcChannel > 7){
        return ERROR_INVALID_ARGUMENT;
    }

    int adc_channel = A0+adcChannel;

    for (int i=0; i<PWM_SAMPLES; i++){
        pulseLenght += pulseIn(adc_channel, HIGH, PWM_SAMPLE_TIMEOUT_US);
        delay(50);
    }

    *output = pulseLenght/PWM_SAMPLES;

    return NO_ERROR;
}
