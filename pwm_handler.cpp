#include "pwm_handler.h"

#include "at_handler.h"

#define MAX_ADC_NUMBER 7
#define PWM_SAMPLES 5
#define PWM_SAMPLE_TIMEOUT_US 1000000

chAT::CommandStatus PwmHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int adcChannel = atoi(parser.args[0].c_str());
    if(adcChannel < 0 || adcChannel > MAX_ADC_NUMBER){
        srv.write_response_prompt();
        srv.write_str("Invalid ADC channel");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    int pulseLenght = 0;
    if(read_pwm_in(adcChannel, &pulseLenght) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error reading ADC");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(String(pulseLenght).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus PwmHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 2){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int pwmChannel = atoi(parser.args[0].c_str());
    int dutyCycle = atoi(parser.args[1].c_str());

    if(set_pwm_out(pwmChannel, dutyCycle) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error writing PWM");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(("#"+String(pwmChannel)+" DutyCycle %:"+String(dutyCycle)).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

TesterError PwmHandler::read_pwm_in(int adcChannel, int *output) {
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

TesterError PwmHandler::set_pwm_out(int pwmChannel, int dutyCycle_percentage) {
    TesterError opStatus = NO_ERROR;
    if(pwmChannel < 2 ||
       pwmChannel > 13 ||
       dutyCycle_percentage < 0 ||
       dutyCycle_percentage > 100)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    int dutyCycle = map(dutyCycle_percentage, 0, 100, 0, 255);
    Serial.print("Duty cycle: ");
    Serial.println(dutyCycle);
    analogWrite(pwmChannel, dutyCycle);

    return NO_ERROR;
}
