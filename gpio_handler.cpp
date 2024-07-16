#include "gpio_handler.h"

#define GPIO_PATH "/sys/class/gpio"

GpioHandler::GpioHandler() {
    this->report = "";
    this->initialized = false;
}

GpioHandler::~GpioHandler() {
}

TesterError GpioHandler::initTests() {
    TesterError opStatus = NO_ERROR;
    String retStr = "", expectedRet = GPIO_PATH;

    opStatus = PTRINT.sendCommand("cd "+String(GPIO_PATH), &retStr,
                                                &expectedRet, true, true);

    if(opStatus == NO_ERROR){
        this->initialized = true;
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
        if(opStatus == NO_ERROR){
            if(testInput) {
                this->portentaInputTestRoutine(portentaGpio, gigaGpio);
            } else {
                this->portentaOutputTestRoutine(portentaGpio, gigaGpio);
            }
        }
    }else{
        opStatus = ERROR_INIT_FAILED;
    }

    return opStatus;
}

String GpioHandler::getReport() {
    String ret = this->report;
    this->report = "";
    return ret;
}

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

TesterError GpioHandler::readPortentaGpio(String gpio, int *value) {
    TesterError opStatus = NO_ERROR;
    String retStr = "";

    int gpiochip = gpio.toInt() / 32;
    int gpioLine = gpio.toInt() % 32;

    opStatus = PTRINT.sendCommand("gpioget "+String(gpiochip)+" "+String(gpioLine), &retStr, NULL, true, true);

    if(opStatus == NO_ERROR){
        *value = retStr.toInt();
    }

    return opStatus;
}

TesterError GpioHandler::writePortentaGpio(String gpio, int value) {
    TesterError opStatus = NO_ERROR;
    String ret = "";

    if(value < 0 || value > 1){
        return ERROR_INVALID_ARGUMENT;
    }

    int gpiochip = gpio.toInt() / 32;
    int gpioLine = gpio.toInt() % 32;

    opStatus = PTRINT.sendCommand("gpioset "+String(gpiochip)+" "+String(gpioLine)+"="+String(value), &ret, NULL, true, true);
    return opStatus;
}

TesterError GpioHandler::startPortentaInterruptCmd(String gpio) {
    TesterError opStatus = NO_ERROR;
    int gpiochip = gpio.toInt() / 32;
    int gpioLine = gpio.toInt() % 32;
    String ret = "";

    opStatus = PTRINT.sendCommand("gpiomon "+String(gpiochip)+" "+String(gpioLine) , &ret, NULL, false, true);
    return opStatus;
}

void GpioHandler::portentaInputTestRoutine(String portentaGpio, int gigaGpio) {
    TesterError opStatus = NO_ERROR;
    int portentaGpioStatus = 0;

    pinMode(gigaGpio, OUTPUT);
    digitalWrite(gigaGpio, LOW);
    delay(200);

    // Simple input test
    opStatus = this->readPortentaGpio(portentaGpio, &portentaGpioStatus);
    if(opStatus == NO_ERROR && portentaGpioStatus == 0){
        digitalWrite(gigaGpio, HIGH);
        delay(200);

        opStatus = this->readPortentaGpio(portentaGpio, &portentaGpioStatus);
        if(opStatus == NO_ERROR && portentaGpioStatus == 1){
            this->report += "GPIO " + portentaGpio + " input test passed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " input test failed\n";
        }
    }else{
        this->report += "GPIO " + portentaGpio + " input test failed\n";
    }

    // Interrupt test
    opStatus = this->startPortentaInterruptCmd(portentaGpio);
    if(opStatus == NO_ERROR){
        String edgeResult = "";
        bool edgeDetected = false;

        digitalWrite(gigaGpio, LOW);
        delay(100);
        digitalWrite(gigaGpio, HIGH);
        delay(100);
        digitalWrite(gigaGpio, LOW);
        delay(100);
        digitalWrite(gigaGpio, HIGH);
        delay(1000); //need this to get the full output

        edgeResult = PTRINT.rawRead();
        Serial.println("[D]-------------------------------");
        Serial.println("Edge result: "+edgeResult);
        Serial.println("[D]-------------------------------");
        if(edgeResult.indexOf("RISING") != -1){
            this->report += "GPIO " + portentaGpio + " interrupt rising edge test passed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " interrupt rising edge test failed\n";
        }

        if(edgeResult.indexOf("FALLING") != -1){
            this->report += "GPIO " + portentaGpio + " interrupt falling edge test passed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " interrupt falling edge test failed\n";
        }
        opStatus = PTRINT.sendCommand(CTRL_C, &edgeResult, NULL, false, false);
    }
}

void GpioHandler::portentaOutputTestRoutine(String portentaGpio, int gigaGpio) {
    TesterError opStatus = NO_ERROR;
    pinMode(gigaGpio, INPUT);

    opStatus = this->writePortentaGpio(portentaGpio, 0);
    delay(200);

    if(opStatus == NO_ERROR && digitalRead(gigaGpio) == LOW){
        opStatus = this->writePortentaGpio(portentaGpio, 1);
        delay(200);

        if(opStatus == NO_ERROR && digitalRead(gigaGpio) == HIGH){
            this->report += "GPIO " + portentaGpio + " output test passed\n";
        }else{
            this->report += "GPIO " + portentaGpio + " output test failed\n";
        }
    }else{
        this->report += "GPIO " + portentaGpio + " output test failed\n";
    }
}
