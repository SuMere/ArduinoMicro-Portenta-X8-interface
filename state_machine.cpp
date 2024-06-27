#include "state_machine.h"

String fromStateToString(TesterState state) {
    switch (state)
    {
    case STATE_INIT:
        return "STATE_INIT";
        break;
    case STATE_WAIT_LOGIN:
        return "STATE_WAIT_LOGIN";
        break;
    case STATE_WAIT_PASSWORD:
        return "STATE_WAIT_PASSWORD";
        break;
    case STATE_LOGGED_IN:
        return "STATE_LOGGED_IN";
        break;
    case STATE_SUDO:
        return "STATE_SUDO";
        break;
    case STATE_IDLE:
        return "STATE_IDLE";
        break;
    case STATE_WAIT_FOR_MODE:
        return "STATE_WAIT_FOR_MODE";
        break;
    case STATE_MODE:
        return "STATE_MODE";
        break;
    case STATE_ERROR:
        return "STATE_ERROR";
        break;
    default:
        return "STATE_UNKNOWN";
        break;
    }
}

StateMachine::StateMachine() {
    this -> state = STATE_INIT;
    this -> currentError = NO_ERROR;
}

StateMachine::StateMachine(TesterState state) {
    this->state = state;
}

StateMachine::~StateMachine() {
}

TesterError StateMachine::init() {
    this -> portentaInterface = PortentaInterface();
    this -> setState(STATE_WAIT_LOGIN);


    return NO_ERROR;
}

TesterError StateMachine::update() {
    switch(state) {
        case STATE_INIT:
            this->init();
            break;

        case STATE_WAIT_LOGIN:
            this->handleStateWaitLogin();
            break;

        case STATE_WAIT_PASSWORD:
            this->hadleStateWaitPassword();
            break;

        case STATE_LOGGED_IN:
            this->handleStateLoggedIn();
            break;

        case STATE_SUDO:
            this->handleStateSudo();
            break;

        case STATE_IDLE:
            this->handleStateIdle();
            break;

        case STATE_WAIT_FOR_MODE:
            this->handleStateWaitForMode();
            break;

        case STATE_MODE:
            this->handleStateMode();
            break;

        case STATE_ERROR:
            this->handleStateError();
            break;
    }
}

TesterState StateMachine::getState() {
    return state;
}

void StateMachine::setState(TesterState state) {
    Serial.println("[>]-----------------------------------------------------------------------------------[>]");
    Serial.println("[>]- STATE CHANGE: "+
                   fromStateToString(this->state)+
                   " -> "+fromStateToString(state));
    Serial.println("[>]-----------------------------------------------------------------------------------[>]");
    this->state = state;
}

void StateMachine::handleStateWaitLogin() {
    this->currentError = this->portentaInterface.waitForLogin();
    
    if(this->currentError == NO_ERROR) {
        String ret = "";
        String waitFor = PSW_PROMPT;
        this->portentaInterface.sendCommand(USR_STRING, &ret, &waitFor, true, true);

        if(this->currentError == NO_ERROR) {
            this->setState(STATE_WAIT_PASSWORD);
            this->afterPswState = STATE_LOGGED_IN;
        } else {
            this->setState(STATE_ERROR);
        }
    } else {
        this->setState(STATE_ERROR);
    }
}

void StateMachine::hadleStateWaitPassword() {
    String ret = "";
    String waitFor = IN_SHELL_PROMPT;
    this->currentError = this->portentaInterface.sendCommand(PSW_STRING, &ret, &waitFor, true, false);

    if(this->currentError == NO_ERROR) {
        this->setState(this->afterPswState);
    } else {
        this->setState(STATE_ERROR);
    }
}

void StateMachine::handleStateLoggedIn() {
    String ret = "";
    String waitFor = PSW_PROMPT;

    Serial.println("[<0>] ------------ Reading raw data");
    Serial.println(this->portentaInterface.rawRead());
    Serial.println("[<0>] ------------ End of raw data");

    delay(3000);
    this->currentError = this->portentaInterface.sendCommand("sudo -s", &ret, &waitFor, true);

    if(this->currentError == NO_ERROR) {
        this->setState(STATE_WAIT_PASSWORD);
        this->afterPswState = STATE_SUDO;
    } else {
        this->setState(STATE_ERROR);
    }
}

void StateMachine::handleStateSudo() {
    String waitFor = IN_SHELL_PROMPT;
    String ret;
    this->currentError = this->portentaInterface.sendCommand("dmesg -n 1", &ret, &waitFor, true, false);
    
    this -> portentaInterface.rawRead();
    if(this->currentError == NO_ERROR) {
        this->setState(STATE_IDLE);
    } else {
        this->setState(STATE_ERROR);
    }
}

void StateMachine::handleStateIdle() {
    String ret;
    this->portentaInterface.sendCommand("whoami", &ret, NULL, true, true);

    Serial.println("[<0>] ------------ WHOAMI TEST ------------");
    Serial.print("Whoami: ");
    Serial.println(ret);
    Serial.println("[<0>] ------------ END OF WHOAMI TEST ------------");
    this->portentaInterface.rawRead();
    this->currentError = ERROR_INVALID_RESPONSE;
    this->setState(STATE_ERROR);
}

void StateMachine::handleStateError() {
    String message = "[!]- ERROR: ";

    Serial.println(message);

    switch (this->currentError)
    {
    case ERROR_INIT_FAILED:
        Serial.println("Error init failed");
        break;
    case ERROR_TIMEOUT:
        Serial.println("Error timeout");
        break;
    case ERROR_INVALID_STATE:
        Serial.println("Error invalid state");
        break;
    case ERROR_INVALID_COMMAND:
        Serial.println("Error invalid command");
        break;
    case ERROR_INVALID_ARGUMENT:
        Serial.println("Error invalid argument");
        break;
    case ERROR_INVALID_RESPONSE:
        Serial.println("Error invalid response");
        break;
    case NO_ERROR:
        Serial.println("No error");
        this->setState(STATE_IDLE);
        break;
    default:
        break;
    }

    Serial.println("@@@@ Waiting for reset... @@@@");
    while(1);
}



    


}

}

}

}
