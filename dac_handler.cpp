#include "dac_handler.h"

#include "at_handler.h"

#define GIGA_DAC A12

DacHandler::DacHandler(CAtHandler *parent)
    : CmdHandler(parent)
{
    parent->registerCommands("+DAC", this);
}

chAT::CommandStatus DacHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    float voltage = atof(parser.args[0].c_str());

    if(set_output_voltage(voltage) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error writing DAC");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(String(voltage).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
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