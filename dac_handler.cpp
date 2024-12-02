#include "dac_handler.h"

#include "at_handler.h"

#define GIGA_DAC A12

chAT::CommandStatus DacHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1){
        return write_error_message(srv, "Invalid number of arguments");
    }

    float voltage = atof(parser.args[0].c_str());

    if(set_output_voltage(voltage) != NO_ERROR){
        return write_error_message(srv, "Error writing DAC");
    }

    return write_ok_message(srv, String(voltage).c_str());
}

int DacHandler::from_voltage_to_dac_count(float voltage) {
    int count = static_cast<int>(voltage * 255 / 3.3);
    return count;
}

TesterError DacHandler::set_output_voltage(float voltage) {
    if (voltage < 0 || voltage > 3.3) {
        return ERROR_INVALID_ARGUMENT;
    }
    
    int count = this->from_voltage_to_dac_count(voltage);

    pinMode(GIGA_DAC, OUTPUT);
    analogWrite(GIGA_DAC, count);
    return NO_ERROR;
}