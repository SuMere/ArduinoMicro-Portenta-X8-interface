#pragma once

#include "common.h"
#include <Arduino_CAN.h>

#define MESSAGE_COUNT 100
#define BITRATE (CanBitRate::BR_1000k)

class CanHandler {
    public:
        CanHandler();
        ~CanHandler();
        TesterError sendFrame(const uint8_t *frame, const uint8_t size);
        TesterError receiveFrame(String *frame, size_t *size);
        TesterError setBitrate(CanBitRate bitrate);
    private:
        static const uint32_t ID = 0x20;
};