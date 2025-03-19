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
    if(uart_data[uart_number].is_configured == false) {
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

    if(uart_data[uart_number].is_configured == false) {
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

    uart_handler_data_t *curr = &uart_data[uart_number];

    String message = "";
    if(uart_data[uart_number].is_configured == false) {
        message = "UART"+String(uart_number)+" not configured";
    }else{
        message = "UART"+String(uart_number)+" is configured as: ";
        message += String(curr->baud_rate)+" "+String(curr->data_bits)+curr->parity+String(curr->stop_bits);

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
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return -EINVAL;
    }

    uart_handler_data_t *curr = &uart_data[uart_number];
    if(curr->is_configured == false) {
        return -EPERM;
    }

    curr->bus->print(message+"\r\n");
    return 0;
}

int UartHandler::receive_message(int uart_number, String *message) {
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return -EINVAL;
    }

    uart_handler_data_t *curr = &uart_data[uart_number];
    if(curr->is_configured == false) {
        return -EPERM;
    }

    int raw_data = curr->bus->read();
    while (raw_data != -1)
    {
        *message += (char)raw_data;
        raw_data = curr->bus->read();
    }

    //*message = curr->bus->readString();
    return 0;
}

int UartHandler::set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate) {
    if(uart_number < 0 || uart_number >= UART_COUNT) {
        return -EINVAL;
    }

    uart_handler_data_t *curr = &uart_data[uart_number];
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
        return -EINVAL;
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
        return -EINVAL;
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
        return -EINVAL;
    }
    printf("STEP0\n");
    curr->bus->begin(baud_rate, config);
    //TODO IMPROVE LOGIC ABOUT TIMEOUT
    //curr->bus->setTimeout(5000);
    //Serial.begin()
    printf("STEP1\n");
    curr->baud_rate = baud_rate;
    curr->data_bits = data_bits;
    curr->parity = parity;
    curr->stop_bits = stop_bits;
    curr->is_configured = true;
    return 0;
}
