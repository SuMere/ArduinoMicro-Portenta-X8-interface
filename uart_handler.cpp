#include "uart_handler.h"

#include "at_handler.h"

UartHandler::UartHandler(CAtHandler *parent) 
    : CmdHandler(parent) 
{
    parent->registerCommands("+UART", this);
}

chAT::CommandStatus UartHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int uart_number = atoi(parser.args[0].c_str());
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        srv.write_response_prompt();
        srv.write_str("Invalid UART number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    String message = "";
    if(is_configured(uart_number) == false) {
        srv.write_response_prompt();
        srv.write_str("Error UART not configured");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    if(receive_message(uart_number, &message) != NO_ERROR) {
        srv.write_response_prompt();
        srv.write_str("Error receiving message");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(message.c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus UartHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 2) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int uart_number = atoi(parser.args[0].c_str());
    String message = parser.args[1].c_str();
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        srv.write_response_prompt();
        srv.write_str("Invalid UART number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    if(is_configured(uart_number) == false) {
        srv.write_response_prompt();
        srv.write_str("Error UART not configured");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    if(send_message(uart_number, message) != NO_ERROR) {
        srv.write_response_prompt();
        srv.write_str("Error sending message");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    return chAT::CommandStatus::OK;
}

chAT::CommandStatus UartHandler::handle_cfg_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int uart_number = atoi(parser.args[0].c_str());
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        srv.write_response_prompt();
        srv.write_str("Invalid UART number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    if(is_configured(uart_number) == false) {
        srv.write_response_prompt();
        srv.write_str("UART not configured");
        srv.write_line_end();
        return chAT::CommandStatus::OK;
    }

    srv.write_response_prompt();
    srv.write_str("UART is configured");
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus UartHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 5) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int uart_number = atoi(parser.args[0].c_str());
    if (uart_number < 0 || uart_number >= UART_COUNT) {
        srv.write_response_prompt();
        srv.write_str("Invalid UART number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    
    int data_bits = atoi(parser.args[1].c_str());
    int stop_bits = atoi(parser.args[2].c_str());
    String parity = parser.args[3].c_str();
    int baud_rate = atoi(parser.args[4].c_str());

    if(set_configuration(uart_number, data_bits, stop_bits, parity, baud_rate) != NO_ERROR) {
        srv.write_response_prompt();
        srv.write_str("Error configuring UART");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    return chAT::CommandStatus::OK;
}

TesterError UartHandler::send_message(int uart_number, String message) {
    String final_message = message + "\n\r";
    switch (uart_number)
    {
    case 0:
        if(is_confgured[uart_number]){
            return ERROR_INVALID_STATE;
        }
        Serial1.print(final_message);
        break;
    case 1:
        if(is_confgured[uart_number]){
            return ERROR_INVALID_STATE;
        }
        Serial2.print(final_message);
        break;
    case 2:
        if(is_confgured[uart_number]){
            return ERROR_INVALID_STATE;
        }
        Serial3.print(final_message);
        break;
    case 3:
        if(is_confgured[uart_number]){
            return ERROR_INVALID_STATE;
        }
        Serial4.print(final_message);
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }
    return NO_ERROR;
}

TesterError UartHandler::receive_message(int uart_number, String *message) {
    switch (uart_number)
    {
    case 0:
        if(this->is_configured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial1.readString();
        break;
    case 1:
        if(this->is_configured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial2.readString();
        break;
    case 2:
        if(this->is_configured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial3.readString();
        break;
    case 3:
        if(this->is_configured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial4.readString();
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }
    return NO_ERROR;
}

TesterError UartHandler::set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate) {
    uint16_t config = 0;

    switch (data_bits)
    {
    case 5:
        config |= SERIAL_DATA_5;
        break;
    case 6:
        config |= SERIAL_DATA_6;
        break;
    case 7:
        config |= SERIAL_DATA_7;
        break;
    case 8:
        config |= SERIAL_DATA_8;
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    switch (stop_bits)
    {
    case 1:
        config |= SERIAL_STOP_BIT_1;
        break;
    case 2:
        config |= SERIAL_STOP_BIT_2;
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    if (parity == "N")
    {
        config |= SERIAL_PARITY_NONE;
    }
    else if (parity == "E")
    {
        config |= SERIAL_PARITY_EVEN;
    }
    else if (parity == "O")
    {
        config |= SERIAL_PARITY_ODD;
    }
    else
    {
        return ERROR_INVALID_ARGUMENT;
    }

    switch (uart_number)
    {
    case 0:
        Serial1.begin(baud_rate, config);
        Serial1.setTimeout(5000);
        break;
    case 1:
        Serial2.begin(baud_rate, config);
        Serial2.setTimeout(5000);
        break;
    case 2:
        Serial3.begin(baud_rate, config);
        Serial3.setTimeout(5000);
        break;
    case 3:
        Serial4.begin(baud_rate, config);
        Serial4.setTimeout(5000);
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    is_confgured[uart_number] = true;

    return NO_ERROR;
}

bool UartHandler::is_configured(int uart_number) {
    if (uart_number < 0 || uart_number >= UART_COUNT) {
        return false;
    }

    return is_confgured[uart_number];
}