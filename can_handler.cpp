#include "can_handler.h"

#include "at_handler.h"

#define TIMEOUT 10000
#define MESSAGE_COUNT 100
#define BITRATE (CanBitRate::BR_1000k)

chAT::CommandStatus CanHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser) {
    if (parser.args.size() != 0) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    String frame;
    size_t size;

    if (receive_frame(&frame, &size) != 0) {
        return write_error_message(srv, "Error receiving frame");
    }

    return write_ok_message(srv, ("Received message is: "+frame).c_str());
}

chAT::CommandStatus CanHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser) {
    if(parser.args.size() != 1){
        return write_error_message(srv, "Invalid number of arguments");
    }

    uint8_t *frame = (uint8_t *)malloc(parser.args[0].length()/2);
    if (frame == NULL){
        return write_error_message(srv, "Memory allocation error");
    }

    for (int i = 0; i < parser.args[0].length(); i+=2){
        frame[i/2] = (uint8_t)strtol(parser.args[0].substr(i, 2).c_str(), NULL, 16);
    }

    if (send_frame(frame, parser.args[0].length()/2) != 0){
        return write_error_message(srv, "Error sending frame");
    }

    return write_ok_message(srv, "");
}

chAT::CommandStatus CanHandler::handle_test(chAT::Server &srv, chAT::ATParser &parser) {
    if (parser.args.size() != 0) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    String message = "\n";
    message += "AT+CAN? - Read CAN message from CAN0 bus\n";
    message += "AT+CAN=<message> - Write CAN message in CAN0 bus\n";
    message += "AT+CAN_CFG=<bitrate> - Configure CAN0 bus speed with <bitrate>\n";

    return write_ok_message(srv, message.c_str());
}

chAT::CommandStatus CanHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser) {
    if (parser.args.size() != 1) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    CanBitRate bitrate = (CanBitRate)atoi(parser.args[0].c_str());
    if (set_bitrate(bitrate) != 0) {
        return write_error_message(srv, "Error setting bitrate");
    }

    is_configured = true;
    return write_ok_message(srv, "");
}

int CanHandler::send_frame(const uint8_t *frame, const uint8_t size) {
    CanMsg message(CanStandardId(this->ID), size, frame);

    int ret = CAN.write(message);
    if(ret <= 0) {
        return -EIO;
    }

    return 0;
}

int CanHandler::receive_frame(String* frame, size_t* size) {
    CanMsg message;

    unsigned long start = millis();

    while (!CAN.available() && millis() - start < TIMEOUT) {
        delay(1);
    }

    if(CAN.available() == 0) {
        return -EIO;
    }

    message = CAN.read();

    for (int i = 0; i < message.data_length; i++){
        *frame += String(message.data[i], HEX);
    }
    *size = message.data_length;

    return 0;
}

int CanHandler::set_bitrate(CanBitRate bitrate) {
    CAN.end();
    CAN.begin(bitrate);
    return 0;
}