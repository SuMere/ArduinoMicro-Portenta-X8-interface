#pragma once

#include <Arduino.h>
#include "cmd_handler.h"
#include "serial_fc.h"
#include <pinDefinitions.h>
#define UART_COUNT 4

typedef struct uart_handler_data_t
{
    HardwareSerial *bus;
    bool is_configured;
    uint32_t baud_rate;
    uint8_t data_bits;
    String parity;
    uint8_t stop_bits;
};

class UartHandler : public CmdHandler {
    private:
        bool is_configured[UART_COUNT] = {false};
        // TX, RX, RTS, CTS
        UART SerialFour = UART(PinNameToIndex(PG_14),
                               PinNameToIndex(PC_7),
                               PinNameToIndex(PG_12),
                               PinNameToIndex(PG_13)
                              );

        uart_handler_data_t uart_data[UART_COUNT] = {
            {&Serial1,    false, 0, 0, "", 0},
            {&Serial2,    false, 0, 0, "", 0},
            {&Serial3,    false, 0, 0, "", 0},
            {&SerialFour, false, 0, 0, "", 0}
        };

        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_test(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        int send_message(int uart_number, String message);
        int receive_message(int uart_number, String *message);
        int set_configuration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate);

    public:
        UartHandler() : CmdHandler("+UART") {}
        virtual ~UartHandler() {}
};
