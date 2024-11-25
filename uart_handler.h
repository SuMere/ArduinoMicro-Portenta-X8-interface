#pragma once

#include "common.h"

#define UART_NUM 4

class UartHandler {
    public:
        UartHandler();
        ~UartHandler();
        TesterError sendMessage(int uart_number, String message);
        TesterError receiveMessage(int uart_number, String *message);
        TesterError setConfiguration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate);
        bool isConfigured(int uart_number);
    private:
};