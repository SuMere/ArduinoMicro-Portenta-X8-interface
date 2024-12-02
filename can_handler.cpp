#include "can_handler.h"

#include "at_handler.h"

#define TIMEOUT 10000
#define MESSAGE_COUNT 100
#define BITRATE (CanBitRate::BR_1000k)

chAT::CommandStatus CanHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser) {
    if (parser.args.size() != 0) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    String frame;
    size_t size;

    if (receive_frame(&frame, &size) != NO_ERROR) {
        srv.write_response_prompt();
        srv.write_str("Error receiving frame");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    srv.write_response_prompt();
    srv.write_str(("Received message is: "+frame).c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus CanHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser) {
    if(parser.args.size() != 1){
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t *frame = (uint8_t *)malloc(parser.args[0].length()/2);
    if (frame == NULL){
        srv.write_response_prompt();
        srv.write_str("Memory allocation error");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    for (int i = 0; i < parser.args[0].length(); i+=2){
        frame[i/2] = (uint8_t)strtol(parser.args[0].substr(i, 2).c_str(), NULL, 16);
    }

    if (send_frame(frame, parser.args[0].length()/2) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error sending frame");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    return chAT::CommandStatus::OK;
}

chAT::CommandStatus CanHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser) {
    if (parser.args.size() != 1) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    CanBitRate bitrate = (CanBitRate)atoi(parser.args[0].c_str());
    if (set_bitrate(bitrate) != NO_ERROR) {
        srv.write_response_prompt();
        srv.write_str("Error setting bitrate");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    is_configured = true;
    return chAT::CommandStatus::OK;
}

TesterError CanHandler::send_frame(const uint8_t *frame, const uint8_t size) {
    CanMsg message(CanStandardId(this->ID), size, frame);

    int ret = CAN.write(message);
    if(ret <= 0) {
        return ERROR_INTERNAL;
    }

    return NO_ERROR;
}

TesterError CanHandler::receive_frame(String* frame, size_t* size) {
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

TesterError CanHandler::set_bitrate(CanBitRate bitrate) {
    CAN.end();
    CAN.begin(bitrate);
    return NO_ERROR;
}