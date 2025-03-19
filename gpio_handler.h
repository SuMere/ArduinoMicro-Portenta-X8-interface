#pragma once

#include "cmd_handler.h"

class GpioHandler : public CmdHandler {
    private:
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_test(chAT::Server&, chAT::ATParser&);

        int get_gpio_status(int gpio, int *value);
        int set_gpio_status(int gpio, int value);

    public:
        GpioHandler() : CmdHandler("+GPIO") {}
        virtual ~GpioHandler() {}
};
