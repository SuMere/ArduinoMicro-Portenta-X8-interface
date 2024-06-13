#include "portenta_interface.h"

PortentaInterface::PortentaInterface() {
}

PortentaInterface::~PortentaInterface() {
}

TesterError waitForString(String str, unsigned long timem, String *ret) {
    String current = "";
    TesterError err = NO_ERROR;

    while(current.indexOf(str) < 0 && millis() - time < COMMAND_TIMEOUT) {
        if(Serial1.available()) {
            current = Serial1.readStringUntil('\n');
            Serial.println(current);
        }
    }

    if(current.indexOf(str) < 0){
        Serial.println("[!]- STRING NOT FOUND");
        err = ERROR_TIMEOUT;
    }else{
        Serial.println("[*]- STRING FOUND");
    }

    *ret = current;

    return err;
}

TesterError PortentaInterface::sendCommand(String cmd, String *ret, String *expectedRet,
                                           bool waitForResponse, bool waitForEcho) 
{
    TesterError err = NO_ERROR;
    String completeCmd = cmd + "\r\n";
    String current = "";

    Serial.print("[*]- SENDING COMMAND ");

    Serial.println(completeCmd);

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
                err = ERROR_TIMEOUT;
            }
        }

        if(err == NO_ERROR && waitForEcho){
            Serial.println("[*]- WAITING FOR ECHO");
            err = waitForString(cmd, this -> time, ret);
        }

        if (err == NO_ERROR && expectedRet){
            Serial.println("[*]- WAITING FOR EXPECTED RESPONSE");
            err = waitForString(*expectedRet, this -> time, ret);
        }
    }
    return err;
}

TesterError PortentaInterface::waitForLogin() {
    TesterError err = ERROR_TIMEOUT;

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
                err = NO_ERROR;
                break;
            }
        Serial.println(current);
        }
    }

    return err;
}

String PortentaInterface::rawRead() {
    String ret = "";
    while(Serial1.available()) {
        ret += Serial1.readString();
    }
    return ret;
}

void PortentaInterface::interactiveMode() {
    String cmd = "";
    while(1) {
        if(Serial.available()) {
            cmd = Serial.readStringUntil('\n');
            String ret;
            if(cmd.indexOf("done") < 0){
                sendCommand(cmd, &ret, NULL, true);
                Serial.println(ret);
            }else{
                break;
            }

        }
    }
}

void PortentaInterface::readUnusedOutput() {
    Serial.println("FLUSHING");
    while(Serial1.available()) {
        Serial.print("********* ");
        Serial.println(Serial1.readStringUntil('\n'));
    }
}