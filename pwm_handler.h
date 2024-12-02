#pragma once

#include "common.h"

#include "cmd_handler.h"

class PwmHandler : public CmdHandler {
    private:
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

        TesterError read_pwm_in(int adcChannel, int *output);
        TesterError set_pwm_out(int pwmChannel, int dutyCycle);
    public:
        PwmHandler() : CmdHandler("+PWM") {}
        virtual ~PwmHandler() {}
};