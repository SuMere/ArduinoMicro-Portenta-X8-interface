#pragma once

#include "common.h"
#include "portenta_interface.h"

class GpioHandler {
    public:
        GpioHandler();
        ~GpioHandler();

        TesterError initTests();
        TesterError testGpio(String portentaGpio, int gigaGpio, 
                             bool testInput);
        String getReport();
        bool isInitialized();

    private:
        String report;
        TesterError toggleGigaGpio(int gpio);
        TesterError togglePortentaGpio(String gpio);

        TesterError readGigaGpio(int gpio, int *value);
        TesterError readPortentaGpio(String gpio, int *value);
        bool initialized;

        bool checkPortentaGpioAvalability(String gpio);
        TesterError exportPortentaGpio(String gpio);
        TesterError unexportPortentaGpio(String gpio);
        TesterError setPortentaGpioDirection(String gpio, bool out);

        void portentaInputTestRoutine(String portentaGpio, int gigaGpio);
        void portentaOutputTestRoutine(String portentaGpio, int gigaGpio);
};
