#pragma once

#include "common.h"

class PortentaInterface {
    public:
        PortentaInterface();
        ~PortentaInterface();
        TesterError waitForLogin();

        TesterError sendCommand(String cmd, String *ret, 
                                String *expectedRet = NULL,
                                bool waitForResponse = false,
                                bool waitForEcho = true);
        String rawRead();
        void interactiveMode();

        String requestInput(String prompt);

    private:
        unsigned long time = 0;
        void readUnusedOutput();
};

extern PortentaInterface PTRINT;
