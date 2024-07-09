#include "gpio_handler.h"

GpioHandler::GpioHandler() {
    this->report = "";
    this->initialized = false;
}

GpioHandler::~GpioHandler() {
}

TesterError GpioHandler::initTests() {
    TesterError opStatus = NO_ERROR;
    String retStr = "", expectedRet = "/sys/class/gpio";

    opStatus = PTRINT.sendCommand("cd /sys/class/gpio", &retStr,
                                                &expectedRet, true, true);

    if(opStatus == NO_ERROR){
    }else{
        opStatus = ERROR_INIT_FAILED;
    }

    return opStatus;
}

TesterError GpioHandler::testGpio(String portentaGpio, int gigaGpio, bool testInput) {
    TesterError opStatus = NO_ERROR;

    if(!this->initialized) {
        return ERROR_INVALID_STATE;
    }

    if(this->checkPortentaGpioAvalability(portentaGpio)){
        opStatus = this->exportPortentaGpio(portentaGpio);
    }

    if(opStatus == NO_ERROR){
        if(testInput) {
            this->portentaInputTestRoutine(portentaGpio, gigaGpio);
        } else {
            this->portentaOutputTestRoutine(portentaGpio, gigaGpio);
        }
    }

    //opStatus = this->unexportPortentaGpio(portentaGpio);

    return opStatus;
}

String GpioHandler::getReport() {
    return this->report;
}


        if(opStatus == NO_ERROR){
        }
    }

    return opStatus;
}

}

TesterError GpioHandler::readPortentaGpio(String gpio, int *value) {
    TesterError opStatus = NO_ERROR;
    String retStr = "";

    opStatus = this->setPortentaGpioDirection(gpio, false);

    if(opStatus == NO_ERROR)
        opStatus = PTRINT.sendCommand("cat gpio" + gpio + "/value", &retStr, NULL, true, true);

    Serial.println("[D] ----------------------------------------------------");
    Serial.println("RET STR" + retStr);
    Serial.println("[D] ----------------------------------------------------");

    if(opStatus == NO_ERROR)
        *value = retStr.toInt();
    else
        *value = -1;

    return opStatus;
bool GpioHandler::isInitialized() {
    return this->initialized;
}

bool GpioHandler::checkPortentaGpioAvalability(String gpio) {
    TesterError opStatus = NO_ERROR;
    String ret = "", retStr = "ls: cannot access";
    bool available = false;

    opStatus = PTRINT.sendCommand("ls gpio" + gpio, &ret, &retStr, true, true);

    if(opStatus == NO_ERROR)
        available = true;
    else
        available = false;

    return available;
}

TesterError GpioHandler::exportPortentaGpio(String gpio) {
    TesterError opStatus = NO_ERROR;

    opStatus = PTRINT.sendCommand("echo " + gpio + " > export", NULL);

    if(!this->checkPortentaGpioAvalability(gpio)){
        Serial.println("[D] - GPIO " + gpio + " exported successfully");
    }else{
        Serial.println("[D] - GPIO " + gpio + " export failed");
        opStatus = ERROR_INVALID_RESPONSE;
    }

    return opStatus;
}

TesterError GpioHandler::unexportPortentaGpio(String gpio) {
    TesterError opStatus = NO_ERROR;

    opStatus = PTRINT.sendCommand("echo " + gpio + " > unexport", NULL);

    return opStatus;
}

TesterError GpioHandler::setPortentaGpioDirection(String gpio, bool out) {
    TesterError opStatus = NO_ERROR;

    if(out)
        opStatus = PTRINT.sendCommand("echo out > gpio" + gpio + "/direction", NULL);
    else
        opStatus = PTRINT.sendCommand("echo in > gpio" + gpio + "/direction", NULL);

    return opStatus;
}

void GpioHandler::portentaInputTestRoutine(String portentaGpio, int gigaGpio) {
    int portentaGpioStatus = 0, gigaGpioStatus = 0;

    pinMode(gigaGpio, OUTPUT);
    digitalWrite(gigaGpio, LOW);
    delay(200);

    this->readPortentaGpio(portentaGpio, &portentaGpioStatus);
    Serial.println("[D] ----------------------------------------------------");
    Serial.println("VALUE BEFORE: " + String(portentaGpioStatus));
    Serial.println("[D] ----------------------------------------------------");

    if(portentaGpioStatus == gigaGpioStatus){
        gigaGpioStatus = (gigaGpioStatus+1)%2;
        digitalWrite(gigaGpio, HIGH);

        delay(200);
        this->readPortentaGpio(portentaGpio, &portentaGpioStatus);
        Serial.println("[D] ----------------------------------------------------");
        Serial.println("VALUE AFTER: " + String(portentaGpioStatus));
        Serial.println("[D] ----------------------------------------------------");
        if(portentaGpioStatus != gigaGpioStatus){
            this->report += "GPIO " + portentaGpio + " test failed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " test passed\n";
        }
    }else{
        this->report += "GPIO " + portentaGpio + " test failed\n";
    }
}

void GpioHandler::portentaOutputTestRoutine(String portentaGpio, int gigaGpio) {
    int portentaGpioStatus = 0, gigaGpioStatus = 0;

    pinMode(gigaGpio, INPUT);
    gigaGpioStatus = digitalRead(gigaGpio);

    this->readPortentaGpio(portentaGpio, &portentaGpioStatus);
    if(portentaGpioStatus == gigaGpioStatus){
        this->togglePortentaGpio(portentaGpio);
        this->readGigaGpio(gigaGpio, &gigaGpioStatus);
        if(portentaGpioStatus != gigaGpioStatus){
            this->report += "GPIO " + portentaGpio + " test failed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " test passed\n";
        }
    }else{
        this->report += "GPIO " + portentaGpio + " test failed\n";
    }
}
