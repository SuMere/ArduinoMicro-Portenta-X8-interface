#include "portenta_interface.h"

TesterError waitForString(String str, time_t time, String *ret) {
    String current = "";
    TesterError opStatus = NO_ERROR;

    while(current.indexOf(str) < 0 && millis() - time < COMMAND_TIMEOUT) {
        if(Serial1.available()) {
            current = Serial1.readStringUntil('\n');
            //Serial.println(current);
        }
    }

    if(current.indexOf(str) < 0){
        Serial.println("[!]- STRING NOT FOUND");
        opStatus = ERROR_TIMEOUT;
    }else{
        Serial.println("[*]- STRING FOUND");
    }

    *ret = current;

    return opStatus;
}

PortentaInterface::PortentaInterface() {}

PortentaInterface::~PortentaInterface() {}

TesterError PortentaInterface::waitForLogin() {
    TesterError opStatus = ERROR_TIMEOUT;

    this -> time = millis();

    while (!Serial1.available()) {
        if (millis() - this -> time > LOGIN_TIMEOUT) {
            return ERROR_TIMEOUT;
        }
    }

    this -> time = millis();

    Serial.println("[*]- WAITING FOR LOGIN");
    while(millis() - this -> time < LOGIN_TIMEOUT){
        if(Serial1.available()) {
            String current = Serial1.readString();
            if (current.indexOf(LOGIN_STRING) > 0){
                Serial.println(current);
                Serial.println("[*]- LOGIN REACHED");
                delay(1000);
                this -> readUnusedOutput();
                opStatus = NO_ERROR;
                break;
            }
        Serial.println(current);
        }
    }

    return opStatus;
}

TesterError PortentaInterface::sendCommand(String cmd, String *ret, String *expectedRet,
                                           bool waitForResponse, bool waitForEcho) 
{
    TesterError opStatus = NO_ERROR;
    String completeCmd = cmd + "\r\n";
    String current = "";

    Serial.print("[*]- SENDING COMMAND ");

    Serial.println(cmd);

    if(expectedRet != NULL && waitForResponse) {
        Serial.print("EXPECTED: ");
        Serial.println(*expectedRet);
    }

    Serial1.print(completeCmd);
    Serial1.flush();

    if(waitForResponse) {

        this -> time = millis();
        while(!Serial1.available()) {
            if (millis() - this -> time > COMMAND_TIMEOUT) {
                opStatus = ERROR_TIMEOUT;
            }
        }

        if(opStatus == NO_ERROR && waitForEcho){
            Serial.println("[*]- WAITING FOR ECHO");
            opStatus = waitForString(cmd, this -> time, ret);
        }

        if (opStatus == NO_ERROR && expectedRet){
            Serial.println("[*]- WAITING FOR EXPECTED RESPONSE");
            opStatus = waitForString(*expectedRet, this -> time, ret);
        }else{
            *ret = Serial1.readStringUntil('\n');
        }

        this -> readUnusedOutput();
    }
    return opStatus;
}

String PortentaInterface::rawRead() {
    String ret = "";
    while(Serial1.available()) {
        ret += Serial1.readString();
    }
    return ret;
}

void PortentaInterface::interactiveMode() {
    Serial.println("INTERACTIVE MODE");
    String tmp="";

    while(true) {
        if(Serial.available()) {
            tmp = Serial.readString();
            if(tmp == "@exit") {
                break;
            }
            Serial1.print(tmp);
           
        }
        this -> readUnusedOutput();
    }
}

String PortentaInterface::requestInput(String prompt) {
    Serial.println(prompt);
    while(!Serial.available()) {delay(100);}
    return Serial.readStringUntil('\n');
}

void PortentaInterface::readUnusedOutput() {
    //Serial.println("FLUSHING");
    while(Serial1.available()) {
        Serial.println(Serial1.read());
    }
}

PortentaInterface PTRINT = PortentaInterface();
