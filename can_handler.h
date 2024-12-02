#pragma once

#include "common.h"

#include "cmd_handler.h"
#include <Arduino_CAN.h>

class CAtHandler;

class CanHandler : public CmdHandler {
    private:
        bool is_configured = false;
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        static const uint32_t ID = 0x20;

        TesterError send_frame(const uint8_t *frame, const uint8_t size);
        TesterError receive_frame(String *frame, size_t *size);
        TesterError set_bitrate(CanBitRate bitrate);

    public:
        CanHandler() : CmdHandler("+CAN") {}
        virtual ~CanHandler() {}
};