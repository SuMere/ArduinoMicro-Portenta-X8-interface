#include "can_handler.h"

#define TIMEOUT 10000

CanHandler::CanHandler() {
    CAN.begin(BITRATE);
}

CanHandler::~CanHandler() {
    CAN.end();
}

TesterError CanHandler::sendFrame(const uint8_t *frame, const uint8_t size) {
    CanMsg message(CanStandardId(this->ID), size, frame);

    int ret = CAN.write(message);
    if(ret <= 0) {
        return ERROR_INTERNAL;
    }

    return NO_ERROR;
}

TesterError CanHandler::receiveFrame(String* frame, size_t* size) {
    CanMsg message;

    unsigned long start = millis();

    while (!CAN.available() && millis() - start < TIMEOUT) {
        delay(1);
    }

    if(CAN.available()) {
        message = CAN.read();
    } else {
        return ERROR_TIMEOUT;
    }

    for (int i = 0; i < message.data_length; i++){
        *frame += String(message.data[i], HEX);
    }
    *size = message.data_length;

    return NO_ERROR;
}

TesterError CanHandler::setBitrate(CanBitRate bitrate) {
    CAN.end();
    CAN.begin(bitrate);
    return NO_ERROR;
}