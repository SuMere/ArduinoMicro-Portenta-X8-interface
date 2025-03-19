#include "helper.h"
#include "at_handler.h"


chAT::CommandStatus Helper::handle_run(chAT::Server &srv, chAT::ATParser &parser)
{
    String message = "";

    message +=  "Welcome to AT command interface\n";
    message +=  "Available commands:\n";
    message +=  "    AT+GPIO\n";
    //message +=  "    AT+ADC\n";
    message +=  "    AT+PWM\n";
    message +=  "    AT+DAC\n";
    message +=  "    AT+UART\n";
    message +=  "    AT+UART_CFG\n";
    message +=  "    AT+I2C\n";
    message +=  "    AT+I2C_CFG\n";
    message +=  "    AT+CAN\n";
    message +=  "    AT+CAN_CFG\n";
    message +=  "\n";
    message +=  "Type AT+<module name>=? (eg. AT+GPIO=?) to get help about the module selected\n";
    return write_ok_message(srv, message.c_str());
}