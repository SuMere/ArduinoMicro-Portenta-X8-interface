#pragma once

#include "common.h"

#include "cmd_handler.h"

class DacHandler : public CmdHandler{
    private:
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);

        TesterError set_output_voltage(float voltage);
        int from_voltage_to_dac_count(float voltage);

    public:
        DacHandler(CAtHandler *parent);
        virtual ~DacHandler() {}
};
