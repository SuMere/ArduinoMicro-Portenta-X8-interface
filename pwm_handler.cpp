#include "pwm_handler.h"

#include "at_handler.h"

#define MAX_ADC_NUMBER 7
#define PWM_SAMPLES 5
#define PWM_SAMPLE_TIMEOUT_US 1000000

chAT::CommandStatus PwmHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1){
        return write_error_message(srv, "Invalid number of arguments");
    }

    int adcChannel = atoi(parser.args[0].c_str());
    if(adcChannel < 0 || adcChannel > MAX_ADC_NUMBER){
        return write_error_message(srv, "Invalid ADC channel");
    }
    int pulseLenght = 0;
    if(read_pwm_in(adcChannel, &pulseLenght) != 0){
        return write_error_message(srv, "Error reading ADC");
    }

    return write_ok_message(srv, String(pulseLenght).c_str());
}

chAT::CommandStatus PwmHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 2){
        return write_error_message(srv, "Invalid number of arguments");
    }

    int pwmChannel = atoi(parser.args[0].c_str());
    int dutyCycle = atoi(parser.args[1].c_str());

    if(set_pwm_out(pwmChannel, dutyCycle) != 0){
        return write_error_message(srv, "Error writing PWM");
    }
    
    return write_ok_message(srv, ("#"+String(pwmChannel)+" DutyCycle %:"+String(dutyCycle)).c_str());
}

int PwmHandler::read_pwm_in(int adcChannel, int *output) {
    float pulseLenght = 0;
    if(adcChannel < 0 || adcChannel > 7){
        return EINVAL;
    }

    int adc_channel = A0+adcChannel;

    for (int i=0; i<PWM_SAMPLES; i++){
        pulseLenght += pulseIn(adc_channel, HIGH, PWM_SAMPLE_TIMEOUT_US);
        delay(50);
    }

    *output = int(pulseLenght/PWM_SAMPLES);

    return 0;
}

//TODO IMPROVE THIS STARTINH FROM HERE [https://github.com/arduino/ArduinoCore-mbed/blob/main/cores/arduino/wiring_analog.cpp#L45-L99]

int PwmHandler::set_pwm_out(int pwmChannel, int dutyCycle_percentage) {
    if(pwmChannel < 2 ||
       pwmChannel > 13 ||
       dutyCycle_percentage < 0 ||
       dutyCycle_percentage > 100)
    {
        return EINVAL;
    }

    int dutyCycle = map(dutyCycle_percentage, 0, 100, 0, 255);
    analogWrite(pwmChannel, dutyCycle);

    return 0;
}
