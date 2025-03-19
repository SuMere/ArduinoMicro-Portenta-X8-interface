#include "uart_handler.h"

#include "at_handler.h"

chAT::CommandStatus UartHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    int uart_number = atoi(parser.args[0].c_str());
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return write_error_message(srv, "Invalid UART number");
    }

    String message = "";
    if(is_configured[uart_number] == false) {
        return write_error_message(srv, "Error UART not configured");
    }
    if(receive_message(uart_number, &message) != 0) {
        return write_error_message(srv, "Error receiving message");
    }

    message = "Received message: \"" + message + "\"";

    return write_ok_message(srv, message.c_str());
}

chAT::CommandStatus UartHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 2) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    int uart_number = atoi(parser.args[0].c_str());
    String message = parser.args[1].c_str();
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return write_error_message(srv, "Invalid UART number");
    }

    if(is_configured[uart_number] == false) {
        return write_error_message(srv, "Error UART not configured");
    }

    if(send_message(uart_number, message) != 0) {
        return write_error_message(srv, "Error sending message");
    }

    return write_ok_message(srv, "");
}

chAT::CommandStatus UartHandler::handle_test(chAT::Server &srv, chAT::ATParser &parser)
{
    String message = "\n";
    message += "AT+UART?<uart bus number> - Read UART message\n";
    message += "AT+UART=<uart bus number>,<message> - Write UART message\n";
    message += "AT+UART_CFG=<uart bus number>,<baud rate>,<data bits>,<parity>,<stop bits> - Configure UART\n";
    message += "            baud rate: 9600, 19200, 38400, 57600, 115200\n";
    message += "            data bits: 5, 6, 7, 8\n";
    message += "            parity: N, E, O\n";
    message += "            stop bits: 1, 2\n";
    message += "AT+UART_CFG?<uart bus number> - Read UART configuration\n";

    return write_ok_message(srv, message.c_str());
}

chAT::CommandStatus UartHandler::handle_cfg_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 1) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    int uart_number = atoi(parser.args[0].c_str());
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return write_error_message(srv, "Invalid UART number");
    }

    if(is_configured[uart_number] == false) {
        return write_error_message(srv, "UART not configured");
    }

    return write_ok_message(srv, "UART is configured");
}

chAT::CommandStatus UartHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if(parser.args.size() != 5) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    int uart_number = atoi(parser.args[0].c_str());
    if (uart_number < 0 || uart_number >= UART_COUNT) {
        return write_error_message(srv, "Invalid UART number");
    }
    
    int baud_rate = atoi(parser.args[1].c_str());
    int data_bits = atoi(parser.args[2].c_str());
    String parity = parser.args[3].c_str();
    int stop_bits = atoi(parser.args[4].c_str());

    if(set_configuration(uart_number, data_bits, stop_bits, parity, baud_rate) != 0) {
        return write_error_message(srv, "Error configuring UART");
    }

    return write_ok_message(srv, "");
}

int UartHandler::send_message(int uart_number, String message) {
    switch (uart_number)
    {
    case 0:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        Serial1.print(message);
        break;
    case 1:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        Serial2.print(message);
        break;
    case 2:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        Serial3.print(message);
        break;
    case 3:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        Serial4.print(message);
        break;
    default:
        return EINVAL;
    }
    return 0;
}

int UartHandler::receive_message(int uart_number, String *message) {
    switch (uart_number)
    {
    case 0:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        *message = Serial1.readString();
        break;
    case 1:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        *message = Serial2.readString();
        break;
    case 2:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        *message = Serial3.readString();
        break;
    case 3:
        if(is_configured[uart_number] == false){
            return EPERM;
        }
        *message = Serial4.readString();
        break;
    default:
        return EINVAL;
    }
    return 0;
}

int UartHandler::set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate) {
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
        return EINVAL;
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
        return EINVAL;
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
        return EINVAL;
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
        return EINVAL;
    }

    is_configured[uart_number] = true;

    return 0;
}
