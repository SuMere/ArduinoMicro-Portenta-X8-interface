#ifndef CI_COMMANDS_H
#define CI_COMMANDS_H

#include "at_handler.h"
#include "gpio_handler.h"
#include "pwm_handler.h"
#include "dac_handler.h"
#include "can_handler.h"
#include "uart_handler.h"

#define MAX_GPIO_NUMBER 94
#define MAX_ADC_NUMBER 7
#define MAX_UART_NUMBER 4

void CAtHandler::add_cmds_ci() {
    /* ....................................................................... */
    command_table[_GPIO] = [this](auto & srv, auto & parser){
    /* ....................................................................... */
        int interfaceGpio = 0;
        String resp = "";
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Read:{
                if (parser.args.size() != 1){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                interfaceGpio = atoi(parser.args[0].c_str());
                if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
                    srv.write_response_prompt();
                    srv.write_str("Invalid GPIO number");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                GpioHandler handler = GpioHandler();
                int gpioStatus = -1;
                TesterError resp = handler.getGpioStatus(interfaceGpio, &gpioStatus);

                if(resp == NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str(String(gpioStatus).c_str());
                    srv.write_line_end();
                    return chAT::CommandStatus::OK;
                } else {
                    srv.write_response_prompt();
                    srv.write_str("Error reading GPIO");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
            }

            case chAT::CommandMode::Write:{
                if (parser.args.size() != 2){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                interfaceGpio = atoi(parser.args[0].c_str());
                if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
                    srv.write_response_prompt();
                    srv.write_str("Invalid GPIO number");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                } else{
                    int value = atoi(parser.args[1].c_str());
                    GpioHandler handler = GpioHandler();
                    TesterError resp = handler.setGpioStatus(interfaceGpio, value);

                    if (resp == NO_ERROR){
                        srv.write_response_prompt();
                        srv.write_str(("#"+String(interfaceGpio)+" Value:"+String(value)).c_str());
                        srv.write_line_end();
                        return chAT::CommandStatus::OK;
                    } else {
                        srv.write_response_prompt();
                        srv.write_str("Error writing GPIO");
                        srv.write_line_end();
                        return chAT::CommandStatus::ERROR;
                    }
                }
            }

            default:
                srv.write_response_prompt();
                srv.write_str("Invalid command mode");
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_PWM] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        int interfaceAdc = 0;
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Read:{
                if (parser.args.size() != 1){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                interfaceAdc = atoi(parser.args[0].c_str());
                if(interfaceAdc < 0 || interfaceAdc > MAX_ADC_NUMBER){
                    srv.write_response_prompt();
                    srv.write_str("Invalid ADC channel");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                PwmHandler handler = PwmHandler();
                int pulseLenght = 0;
                TesterError resp = handler.readPwmIn(interfaceAdc, &pulseLenght);

                if (resp == NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str(String(pulseLenght).c_str());
                    srv.write_line_end();
                    return chAT::CommandStatus::OK;
                } else {
                    srv.write_response_prompt();
                    srv.write_str("Error reading ADC");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
            }
            case chAT::CommandMode::Write:{
                if (parser.args.size() != 2){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                int pwmChannel = atoi(parser.args[0].c_str());
                int dutyCycle = atoi(parser.args[1].c_str());

                PwmHandler handler = PwmHandler();
                TesterError resp = handler.setPwmOut(pwmChannel, dutyCycle);

                if (resp == NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str(("#"+String(pwmChannel)+" DutyCycle %:"+String(dutyCycle)).c_str());
                    srv.write_line_end();
                    return chAT::CommandStatus::OK;
                } else {
                    srv.write_response_prompt();
                    srv.write_str("Error writing PWM");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
            }

            default:
                srv.write_response_prompt();
                srv.write_str("Invalid command mode");
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_DAC] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Write:{
                if (parser.args.size() != 1){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                float voltage = atof(parser.args[0].c_str());
                DacHandler handler = DacHandler();
                TesterError resp = handler.setOutputVoltage(voltage);

                if (resp == NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str(String(voltage).c_str());
                    srv.write_line_end();
                    return chAT::CommandStatus::OK;
                } else {
                    srv.write_response_prompt();
                    srv.write_str("Error writing DAC");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
            }
            default:
                srv.write_response_prompt();
                srv.write_str("Invalid command mode");
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_CAN] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Read:{
                if(parser.args.size() != 0){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
                String frame = "";
                size_t size = 0; 
                CanHandler handler = CanHandler();
                if (handler.receiveFrame(&frame, &size) != NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str("Error receiving frame");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                srv.write_response_prompt();
                srv.write_str(("received -"+String(size)+"- bytes. ").c_str());
                srv.write_str(("Message is:"+frame).c_str());
                srv.write_line_end();

                return chAT::CommandStatus::OK;
            }

            case chAT::CommandMode::Write:{
                if(parser.args.size() != 1){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                uint8_t *frame = (uint8_t *)malloc(parser.args[0].length()/2);
                if (frame == NULL){
                    srv.write_response_prompt();
                    srv.write_str("Memory allocation error");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                for (int i = 0; i < parser.args[0].length(); i+=2){
                    frame[i/2] = (uint8_t)strtol(parser.args[0].substr(i, 2).c_str(), NULL, 16);
                }
                //frame = (uint8_t *)parser.args[0].c_str();

                CanHandler handler = CanHandler();
                if (handler.sendFrame(frame, parser.args[0].length()/2) != NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str("Error sending frame");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                return chAT::CommandStatus::OK;
            }
            default:
                srv.write_response_prompt();
                srv.write_str("Invalid command mode");
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_I2C] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Test:{

                // call the function to test the I2C
                return chAT::CommandStatus::OK; //maybe this should be a return value from the test function
            }
            default:
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_SPI] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Test:{

                // call the function to test the SPI
                return chAT::CommandStatus::OK; //maybe this should be a return value from the test function
            }
            default:
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_UART] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Read:{
                if(parser.args.size() != 1) {
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                int uart_number = atoi(parser.args[0].c_str());

                UartHandler handler = UartHandler();
                if(handler.isConfigured(uart_number) == false){
                    srv.write_response_prompt();
                    srv.write_str("Error UART not configured");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                String message = "";
                if (handler.receiveMessage(uart_number, &message) != NO_ERROR){
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

            case chAT::CommandMode::Write:{
                if(parser.args.size() != 2){
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                int uart_number = atoi(parser.args[0].c_str());
                String message = parser.args[1].c_str();
                UartHandler handler = UartHandler();
                if(handler.isConfigured(uart_number) == false){
                    srv.write_response_prompt();
                    srv.write_str("Error UART not configured");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                if(handler.sendMessage(uart_number, message) != NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str("Error sending message");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                return chAT::CommandStatus::OK;
            }

            case chAT::CommandMode::Test:{
                if(parser.args.size() != 5) {
                    srv.write_response_prompt();
                    srv.write_str("Invalid number of arguments");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
                int uart_number = atoi(parser.args[0].c_str());
                int data_bits = atoi(parser.args[1].c_str());
                String parity = parser.args[2].c_str();
                int stop_bits = atoi(parser.args[3].c_str());
                int baud_rate = atoi(parser.args[4].c_str());

                UartHandler handler = UartHandler();
                if(handler.setConfiguration(uart_number, data_bits, stop_bits, parity, baud_rate) != NO_ERROR){
                    srv.write_response_prompt();
                    srv.write_str("Error configuring UART");
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                return chAT::CommandStatus::OK;
            }

            default:
                srv.write_response_prompt();
                srv.write_str("Invalid command mode");
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_I2S] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Test:{


                // call the function to test the I2S
                return chAT::CommandStatus::OK; //maybe this should be a return value from the test function
            }
            default:
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_REPORT] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Read:{
                //Call get report
                return chAT::CommandStatus::OK;
            }
            default:
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };

    /* ....................................................................... */
    command_table[_HELP] = [this](auto & srv, auto & parser) {
    /* ....................................................................... */
        switch (parser.cmd_mode) {
            case chAT::CommandMode::Run:{
                String message = String("AT+<MODULE NAME>: run command\r\n")+
                                 String("AT+<MODULE NAME>?<PARAM>: read module parameter\r\n")+
                                 String("AT+<MODULE NAME>=<PARAM>: write module parameter\r\n")+
                                 String("AT+<MODULE NAME>=?<PARAM>: test module parameter\r\n")+
                                 String("Available modules: GPIO, ADC, DAC, CAN, I2C, SPI, UART, I2S\r\n")+
                                 String("Each module has its own helper\r\n")+
                                 String("Send AT+HELP?<MODULE NAME> to get module helper\r\n");
                srv.write_response_prompt();
                srv.write_line_end();
                srv.write_str(message.c_str());
                srv.write_line_end();
                return chAT::CommandStatus::OK;
            }

            case chAT::CommandMode::Read:{
                if (parser.args.size() != 1){
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }
                String message = "";
                String module = parser.args[0].c_str();
                if(module == "GPIO"){
                    String message = String("\tAT+GPIO=<GPIO>,<VALUE>: set GPIO value\r\n")+
                                     String("\tAT+GPIO?<GPIO>: get GPIO value\r\n")+
                                     String("\tGPIO range: 0-80\r\n")+
                                     String("\tVALUE range: 0-1\r\n");
                } else if(module == "ADC"){
                    String message = String("\tAT+ADC?<ADC channel>: get ADC value\r\n")+
                                     String("\tADC range: 0-7\r\n");
                } else if(module == "DAC"){
                    String message = String("\tAT+DAC=<VOLTAGE>: set DAC output voltage\r\n")+
                                     String("\tVOLTAGE range: 0-3.3 volts\r\n");
                } else if(module == "CAN"){
                    String message = String("\tAT+CAN=<DATA>: write data on can bus\r\n")+
                                     String("\tAT+CAN?: read data from can bus\r\n");
                } else{
                    srv.write_line_end();
                    return chAT::CommandStatus::ERROR;
                }

                srv.write_response_prompt();
                srv.write_str(message.c_str());
                srv.write_line_end();
                return chAT::CommandStatus::OK;
            }
            default:
                srv.write_line_end();
                return chAT::CommandStatus::ERROR;
        }
    };
}

#endif