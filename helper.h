#pragma once

#include "cmd_handler.h"

class Helper : public CmdHandler {
    private:
        virtual chAT::CommandStatus handle_run(chAT::Server&, chAT::ATParser&);

    public:
        Helper() : CmdHandler("+HELP") {}
        virtual ~Helper() {}
};