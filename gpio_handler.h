#pragma once

#include "common.h"

#include "cmd_handler.h"

class GpioHandler : public CmdHandler {
    private:
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

        TesterError get_gpio_status(int gpio, int *value);
        TesterError set_gpio_status(int gpio, int value);

    public:
        GpioHandler(CAtHandler *parent);
        virtual ~GpioHandler() {}
};
