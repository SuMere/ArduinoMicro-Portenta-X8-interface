#pragma once

#include "common.h"

#include "cmd_handler.h"

class CAtHandler;

class GpioHandler : public CmdHandler {
    private:
	virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
	virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

	virtual ~GpioHandler() = 0;

	TesterError getGpioStatus(int gpio, int *value);
	TesterError setGpioStatus(int gpio, int value);

    public:
	GpioHandler(CAtHandler *parent); : CmdHandler(parent) {}
};
