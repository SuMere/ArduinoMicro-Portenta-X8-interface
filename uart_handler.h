#pragma once

#include <Arduino.h>
#include "cmd_handler.h"

#define UART_COUNT 4

class UartHandler : public CmdHandler {
    private:
        bool is_confgured[UART_COUNT] = {false};
        
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        int send_message(int uart_number, String message);
        int receive_message(int uart_number, String *message);
        int set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate);
        bool is_configured(int uart_number);

    public:
        UartHandler() : CmdHandler("+UART") {}
        virtual ~UartHandler() {}
};