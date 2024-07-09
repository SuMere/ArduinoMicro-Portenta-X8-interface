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
        bool initialized;

        bool checkPortentaGpioAvalability(String gpio);
        TesterError setPortentaGpioDirection(String gpio, bool out);
        TesterError readPortentaGpio(String gpio, int *value);
        TesterError writePortentaGpio(String gpio, int value);

        void portentaInputTestRoutine(String portentaGpio, int gigaGpio);
        void portentaOutputTestRoutine(String portentaGpio, int gigaGpio);
};
