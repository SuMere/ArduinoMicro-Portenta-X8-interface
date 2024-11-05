#ifndef ARDUINO_AT_HANDLER_H
#define ARDUINO_AT_HANDLER_H

#include "chAT.hpp"
#include "Server.h"
#include "Arduino.h"

using namespace SudoMaker;

class CAtHandler {
private:
   std::unordered_map<std::string, std::function<chAT::CommandStatus(chAT::Server&, chAT::ATParser&)>> command_table;
   chAT::Server at_srv;
   HardwareSerial *serial;

   int last_server_client_sock;

   void add_cmds_ci();
public:
   CAtHandler(HardwareSerial *s);
   CAtHandler() = delete ;
   void run();
};


#endif
