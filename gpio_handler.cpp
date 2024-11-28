#include "gpio_handler.h"

#include "at_handler.h"

#define MAX_GPIO_NUMBER 94

GpioHandler::GpioHandler(CAtHandler *parent)
    : CmdHandler(parent)
{
    parent->registerCommands("+GPIO", this);
}

chAT::CommandStatus GpioHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int interfaceGpio = atoi(parser.args[0].c_str());
    if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
        srv.write_response_prompt();
        srv.write_str("Invalid GPIO number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int gpioStatus = -1;

    if(get_gpio_status(interfaceGpio, &gpioStatus) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error reading GPIO");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(String(gpioStatus).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus GpioHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 2){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    int interfaceGpio = atoi(parser.args[0].c_str());
    if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
        srv.write_response_prompt();
        srv.write_str("Invalid GPIO number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    int value = atoi(parser.args[1].c_str());

    if (set_gpio_status(interfaceGpio, value) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error writing GPIO");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(("#"+String(interfaceGpio)+" Value:"+String(value)).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

TesterError GpioHandler::get_gpio_status(int gpio, int *value)
{
    TesterError opStatus = NO_ERROR;

    pinMode(gpio, INPUT);
    *value = digitalRead(gpio);

    return opStatus;
}

TesterError GpioHandler::set_gpio_status(int gpio, int value)
{
    TesterError opStatus = NO_ERROR;

#ifdef DAC_0 || DAC
    if (gpio == DAC_0 || gpio == DAC) {
        if(value == 1)
            analogWrite(gpio, 255);
        else{
            analogWrite(gpio, 0);
        }
        return opStatus;
    }
#endif

#ifdef DAC_1
    if (gpio == DAC_1) {
        if(value == 1)
            analogWrite(gpio, 255);
        else{
            analogWrite(gpio, 0);
        }
        return opStatus;
    }
#endif

    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, value);

    return opStatus;
}
