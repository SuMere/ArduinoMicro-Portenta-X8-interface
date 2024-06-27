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
    STATE_ERROR
} TesterState;

class StateMachine {
    public:
        StateMachine();
        StateMachine(TesterState state);
        ~StateMachine();
        TesterError init();
        TesterError update();
        void setState(TesterState state);
        TesterState getState();

    private:
        TesterState state;
        TesterState afterPswState;
        
        TesterError currentError;

        PortentaInterface portentaInterface;
        void setState(TesterState state);

        void handleStateWaitLogin();
        void hadleStateWaitPassword();
        void handleStateLoggedIn();
        void handleStateSudo();
        void handleStateIdle();
        void handleStateError();
};