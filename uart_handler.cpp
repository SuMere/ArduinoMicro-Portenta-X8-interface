#include "uart_handler.h"

uint8_t curr_config = 0;

void set_config(uint8_t config){
    curr_config |= config;
}

uint8_t get_config(){
    return curr_config;
}

UartHandler::UartHandler() {
}

UartHandler::~UartHandler() {}

TesterError UartHandler::sendMessage(int uart_number, String message) {
    String final_message = message + "\n\r";
    switch (uart_number)
    {
    case 0:
        if((get_config() & 0x01) == 0){
            return ERROR_INVALID_STATE;
        }
        Serial1.print(final_message);
        break;
    case 1:
        if((get_config() & 0x02) == 0){
            return ERROR_INVALID_STATE;
        }
        Serial2.print(final_message);
        break;
    case 2:
        if((get_config() & 0x04) == 0){
            return ERROR_INVALID_STATE;
        }
        Serial3.print(final_message);
        break;
    case 3:
        if((get_config() & 0x08) == 0){
            return ERROR_INVALID_STATE;
        }
        Serial4.print(final_message);
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }
    return NO_ERROR;
}

TesterError UartHandler::receiveMessage(int uart_number, String *message) {
    switch (uart_number)
    {
    case 0:
        if(this->isConfigured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial1.readString();
        break;
    case 1:
        if(this->isConfigured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial2.readString();
        break;
    case 2:
        if(this->isConfigured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial3.readString();
        break;
    case 3:
        if(this->isConfigured(uart_number) == false){
            return ERROR_INVALID_STATE;
        }
        *message = Serial4.readString();
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }
    return NO_ERROR;
}

TesterError UartHandler::setConfiguration(int uart_number, int data_bits, int stop_bits, String parity, int baud_rate) {
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
        set_config(0x01);
        break;
    case 1:
        Serial2.begin(baud_rate, config);
        Serial2.setTimeout(5000);
        set_config(0x02);
        break;
    case 2:
        Serial3.begin(baud_rate, config);
        Serial3.setTimeout(5000);
        set_config(0x04);
        break;
    case 3:
        Serial4.begin(baud_rate, config);
        Serial4.setTimeout(5000);
        set_config(0x08);
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    return NO_ERROR;
}

bool UartHandler::isConfigured(int uart_number) {
    if (uart_number < 0 || uart_number >= UART_NUM) {
        return false;
    }

    return get_config() && (1 << uart_number);
}