#pragma once

#include "cmd_handler.h"

class PwmHandler : public CmdHandler {
    private:
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

        int read_pwm_in(int adcChannel, int *output);
        int set_pwm_out(int pwmChannel, int dutyCycle);
    public:
        PwmHandler() : CmdHandler("+PWM") {}
        virtual ~PwmHandler() {}
};