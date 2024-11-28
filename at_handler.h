#ifndef ARDUINO_AT_HANDLER_H
#define ARDUINO_AT_HANDLER_H

#include "chAT.hpp"
#include "Server.h"
#include "Arduino.h"

using namespace SudoMaker;

class CmdHandler;

class CAtHandler {
private:
   std::unordered_map<std::string, CmdHandler *> command_table;
   chAT::Server at_srv;
   HardwareSerial *serial;

   void add_cmds_ci();
public:
   CAtHandler();
   void begin(HardwareSerial *s);
   void run();
   void registerCommands(std::string cmd, CmdHandler *handler);
};

CAtHandler &at_handler();

#endif
