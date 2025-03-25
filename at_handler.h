#ifndef ARDUINO_AT_HANDLER_H
#define ARDUINO_AT_HANDLER_H

#include "chAT.hpp"
#include "Server.h"
#include "Arduino.h"

using namespace SudoMaker;

class CmdHandler;

/**
 * @class CAtHandler
 * @brief Handles AT commands and manages the chAT server.
 */
class CAtHandler {
private:
   std::unordered_map<std::string, CmdHandler *> command_table; /**< Table mapping commands to their handlers. */
   chAT::Server at_srv; /**< chAT server instance. */
   HardwareSerial *serial; /**< Pointer to the serial interface. */

   int last_server_client_sock; /**< Last client socket for the server. */

   /**
    * @brief Adds CI-specific commands to the command table.
    */
   void add_cmds_ci();

public:
   /**
    * @brief Initializes the AT handler with the specified serial interface.
    * @param s Pointer to the HardwareSerial interface.
    */
   void begin(HardwareSerial *s);

   /**
    * @brief Constructor for the CAtHandler class.
    */
   CAtHandler();

   /**
    * @brief Runs the AT server, processing incoming commands.
    */
   void run();

   /**
    * @brief Registers a command and its handler.
    * @param cmd The command string.
    * @param handler Pointer to the command handler.
    */
   void registerCommands(std::string cmd, CmdHandler *handler);
};

/**
 * @brief Provides a singleton instance of the CAtHandler class.
 * @return Reference to the singleton CAtHandler instance.
 */
CAtHandler &at_handler();

#endif
