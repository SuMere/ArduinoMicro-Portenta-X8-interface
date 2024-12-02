#include "gpio_handler.h"

#include "at_handler.h"

#define MAX_GPIO_NUMBER 94

chAT::CommandStatus GpioHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1){
        return write_error_message(srv, "Invalid number of arguments");
    }

    int interfaceGpio = atoi(parser.args[0].c_str());
    if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
        return write_error_message(srv, "Invalid GPIO number");
    }

    int gpioStatus = -1;

    if(get_gpio_status(interfaceGpio, &gpioStatus) != NO_ERROR){
        return write_error_message(srv, "Error reading GPIO");
    }
    
    return write_ok_message(srv, ("#"+String(interfaceGpio)+" Value:"+String(gpioStatus)).c_str());
}

chAT::CommandStatus GpioHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 2){
        return write_error_message(srv, "Invalid number of arguments");
    }

    int interfaceGpio = atoi(parser.args[0].c_str());
    if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
        return write_error_message(srv, "Invalid GPIO number");
    }
    int value = atoi(parser.args[1].c_str());

    if (set_gpio_status(interfaceGpio, value) != NO_ERROR){
        return write_error_message(srv, "Error writing GPIO");
    }

    return write_ok_message(srv, ("#"+String(interfaceGpio)+" Value:"+String(value)).c_str());
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

    if (false
#ifdef DAC
	    || gpio == DAC
#endif
#ifdef DAC_0
	    || gpio == DAC_0
#endif
#ifdef DAC_1
	    || gpio == DAC_1
#endif
    ) {
        if(value == 1)
            analogWrite(gpio, 255);
        else{
            analogWrite(gpio, 0);
        }
        return opStatus;
    }

    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, value);

    return opStatus;
}
