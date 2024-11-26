#include "pwm_handler.h"

#define PWM_SAMPLES 5
#define PWM_SAMPLE_TIMEOUT_US 1000000

PwmHandler::PwmHandler() {}

PwmHandler::~PwmHandler() {}

TesterError PwmHandler::readPwmIn(int adcChannel, int *output) {
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

    *output = int(pulseLenght/PWM_SAMPLES);

    return NO_ERROR;
}


//TODO IMPROVE THIS STARTINH FROM HERE [https://github.com/arduino/ArduinoCore-mbed/blob/main/cores/arduino/wiring_analog.cpp#L45-L99]

TesterError PwmHandler::setPwmOut(int pwmChannel, int dutyCycle_percentage) {
    TesterError opStatus = NO_ERROR;
    if(pwmChannel < 2 ||
       pwmChannel > 13 ||
       dutyCycle_percentage < 0 ||
       dutyCycle_percentage > 100)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    int dutyCycle = map(dutyCycle_percentage, 0, 100, 0, 255);
    analogWrite(pwmChannel, dutyCycle);

    return NO_ERROR;
}
