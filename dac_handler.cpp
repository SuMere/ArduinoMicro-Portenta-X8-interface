#include "dac_handler.h"

#include "at_handler.h"

#define GIGA_DAC A12

chAT::CommandStatus DacHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1){
        return write_error_message(srv, "Invalid number of arguments");
    }

    float voltage = atof(parser.args[0].c_str());

    if(set_output_voltage(voltage) != 0){
        return write_error_message(srv, "Error writing DAC");
    }

    return write_ok_message(srv, String(voltage).c_str());
}

chAT::CommandStatus DacHandler::handle_test(chAT::Server &srv, chAT::ATParser &parser)
{
    String message = "\n";
    message += "AT+DAC=<voltage> - Set DAC output voltage in DAC0 pin\n";
    return write_ok_message(srv, message.c_str());
}

int DacHandler::from_voltage_to_dac_count(float voltage) {
    int count = static_cast<int>(voltage * 255 / 3.3);
    return count;
}

int DacHandler::set_output_voltage(float voltage) {
    if (voltage < 0 || voltage > 3.3) {
        return -EINVAL;
    }

    int count = this->from_voltage_to_dac_count(voltage);

    pinMode(GIGA_DAC, OUTPUT);
    analogWrite(GIGA_DAC, count);
    return 0;
}