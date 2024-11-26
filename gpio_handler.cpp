#include "gpio_handler.h"
#include "at_handler.h"

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

	interfaceGpio = atoi(parser.args[0].c_str());
	if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
		srv.write_response_prompt();
		srv.write_str("Invalid GPIO number");
		srv.write_line_end();
		return chAT::CommandStatus::ERROR;
	}

	TesterError resp = getGpioStatus(interfaceGpio, &gpioStatus);

	if(resp != NO_ERROR){
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

	interfaceGpio = atoi(parser.args[0].c_str());
	if(interfaceGpio < 0 || interfaceGpio > MAX_GPIO_NUMBER){
		srv.write_response_prompt();
		srv.write_str("Invalid GPIO number");
		srv.write_line_end();
		return chAT::CommandStatus::ERROR;
	}
	int value = atoi(parser.args[1].c_str());
	TesterError resp = setGpioStatus(interfaceGpio, value);

	if (resp != NO_ERROR){
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

TesterError GpioHandler::getGpioStatus(int gpio, int *value)
{
	TesterError opStatus = NO_ERROR;

	pinMode(gpio, INPUT);
	*value = digitalRead(gpio);

	return opStatus;
}

TesterError GpioHandler::setGpioStatus(int gpio, int value)
{
	TesterError opStatus = NO_ERROR;

#if defined(DAC) || defined(DAC_0) || defined(DAC_1)
	switch (gpio) {
#ifdef DAC
		case DAC:
#endif
#ifdef DAC_0
		case DAC_0:
#endif
#ifdef DAC_1
		case DAC_1:
#endif
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
