#pragma once

#include "common.h"

#include "cmd_handler.h"

#define UART_COUNT 4

class UartHandler : public CmdHandler {
    private:
        bool is_confgured[UART_COUNT] = {false};
        
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        TesterError send_message(int uart_number, String message);
        TesterError receive_message(int uart_number, String *message);
        TesterError set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate);
        bool is_configured(int uart_number);

    public:
        UartHandler(CAtHandler *parent);
        virtual ~UartHandler() {}
};