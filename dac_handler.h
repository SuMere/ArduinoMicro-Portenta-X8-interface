#pragma once

#include "cmd_handler.h"

class DacHandler : public CmdHandler{
    private:
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

        int set_output_voltage(float voltage);
        int from_voltage_to_dac_count(float voltage);

    public:
        DacHandler() : CmdHandler("+DAC") {}
        virtual ~DacHandler() {}
};
