#pragma once

#include "cmd_handler.h"
#include <Arduino_CAN.h>

class CAtHandler;

class CanHandler : public CmdHandler {
    private:
        bool is_configured = false;
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_test(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        static const uint32_t ID = 0x20;

        int send_frame(const uint8_t *frame, const uint8_t size);
        int receive_frame(String *frame, size_t *size);
        int set_bitrate(CanBitRate bitrate);

    public:
        CanHandler() : CmdHandler("+CAN") {}
        virtual ~CanHandler() {}
};