#pragma once

#include "common.h"
#include "portenta_interface.h"

typedef enum {
    STATE_INIT,
    STATE_WAIT_LOGIN,
    STATE_WAIT_PASSWORD,
    STATE_LOGGED_IN,
    STATE_SUDO,
    STATE_IDLE,
    STATE_WAIT_FOR_MODE,
    STATE_MODE,
    STATE_ERROR
} TesterState;

typedef enum {
    MODE_NONE,
    MODE_GPIO,
    MODE_PWM,
    MODE_ADC,
    MODE_I2C,
    MODE_SPI,
    MODE_UART,
    MODE_I2S,
    MODE_INTERACTIVE
}TesterMode;

class StateMachine {
    public:
        StateMachine();
        StateMachine(TesterState state);
        ~StateMachine();
        TesterError init();
        TesterError update();
        TesterState getState();

    private:
        TesterState state;
        TesterState afterPswState;
        TesterMode mode;
        TesterError currentError;

        PortentaInterface portentaInterface;
        void setState(TesterState state);

        void handleStateWaitLogin();
        void hadleStateWaitPassword();
        void handleStateLoggedIn();
        void handleStateSudo();
        void handleStateIdle();
        void handleStateWaitForMode();
        void handleStateMode();
        void handleStateError();

        TesterError gpioMode();
        TesterError pwmMode();
        TesterError adcMode();
        TesterError i2cMode();
        TesterError spiMode();
        TesterError uartMode();
        TesterError i2sMode();
        TesterError interactiveMode();
};
