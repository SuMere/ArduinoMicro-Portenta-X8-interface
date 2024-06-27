#include "state_machine.h"


String fromModeToString(TesterMode mode) {
    switch (mode)
    {
    case MODE_NONE:
        return "MODE_NONE";
        break;
    case MODE_GPIO:
        return "MODE_GPIO";
        break;
    case MODE_PWM:
        return "MODE_PWM";
        break;
    case MODE_ADC:
        return "MODE_ADC";
        break;
    case MODE_I2C:
        return "MODE_I2C";
        break;
    case MODE_SPI:
        return "MODE_SPI";
        break;
    case MODE_UART:
        return "MODE_UART";
        break;
    case MODE_I2S:
        return "MODE_I2S";
        break;
    case MODE_INTERACTIVE:
        return "MODE_INTERACTIVE";
        break;
    default:
        return "MODE_UNKNOWN";
        break;
    }
}

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

    this->setState(STATE_WAIT_FOR_MODE);
}

void StateMachine::handleStateWaitForMode() {
    Serial.println("Select Mode:");
    Serial.println("0: None");
    Serial.println("1: GPIO");
    Serial.println("2: PWM (DISABLED ATM)");
    Serial.println("3: ADC (DISABLED ATM)");
    Serial.println("4: I2C (DISABLED ATM)");
    Serial.println("5: SPI (DISABLED ATM)");
    Serial.println("6: UART (DISABLED ATM)");
    Serial.println("7: I2S (DISABLED ATM)");
    Serial.println("8: Interactive");

    while (!Serial.available()) {
        delay(100);
    }

    this->mode = TesterMode(Serial.readString().toInt());

    this->setState(STATE_MODE);
}

void StateMachine::handleStateMode() {
    switch (this->mode)
    {
    case MODE_NONE:
        this->setState(STATE_WAIT_FOR_MODE);
        break;
    case MODE_GPIO:
        this->currentError = this->gpioMode();
        break;
    case MODE_PWM:
        this->currentError = this->pwmMode();
        break;
    case MODE_ADC:
        this->currentError = this->adcMode();
        break;
    case MODE_I2C:
        this->currentError = this->i2cMode();
        break;
    case MODE_SPI:
        this->currentError = this->spiMode();
        break;
    case MODE_UART:
        this->currentError = this->uartMode();
        break;
    case MODE_I2S:
        this->currentError = this->i2sMode();
        break;
    case MODE_INTERACTIVE:
        this->interactiveMode();
        break;
    default:
        Serial.println("Unidentified mode");
        this->setState(STATE_WAIT_FOR_MODE);
        break;
    }
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

TesterError StateMachine::gpioMode() {
    TesterError opStatus = NO_ERROR;
    String portentaGpio = "";
    int gigaGpio = 0;

    Serial.println("[@] - GPIO MODE");

    
    portentaGpio = PTRINT.requestInput("Insert Portenta GPIO number: ");
   // portentaGpio = "@exit";
    if(portentaGpio.indexOf("@exit") >= 0) {
        this->setState(STATE_WAIT_FOR_MODE);
    } else {
        gigaGpio = PTRINT.requestInput("Insert Giga GPIO number: ").toInt();
        GpioHandler gpioHandler = GpioHandler();
        opStatus = gpioHandler.initTests();
        if(opStatus == NO_ERROR) {
            opStatus = gpioHandler.testGpio(portentaGpio, gigaGpio, true);
        }
    }
    
    if(opStatus != NO_ERROR) {
        this->setState(STATE_ERROR);
    }
    return opStatus;
}

TesterError StateMachine::pwmMode() {
    Serial.println("[@] - PWM MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::adcMode() {
    Serial.println("[@] - ADC MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::i2cMode() {
    Serial.println("[@] - I2C MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::spiMode() {
    Serial.println("[@] - SPI MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::uartMode() {
    Serial.println("[@] - UART MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::i2sMode() {
    Serial.println("[@] - I2S MODE");
    this->setState(STATE_WAIT_FOR_MODE);
}

TesterError StateMachine::interactiveMode() {
    Serial.println("[@] - INTERACTIVE MODE");
    PTRINT.interactiveMode();
    this->setState(STATE_WAIT_FOR_MODE);
}
