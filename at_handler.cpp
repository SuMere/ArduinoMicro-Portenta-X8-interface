

#include "at_handler.h"
#include "commands.h"

#include "cmds_ci.h"

using namespace SudoMaker;

/* -------------------------------------------------------------------------- */
void CAtHandler::run() {
/* -------------------------------------------------------------------------- */   
   at_srv.run();
}

/* -------------------------------------------------------------------------- */
CAtHandler::CAtHandler(HardwareSerial *s) : last_server_client_sock(0) {
/* -------------------------------------------------------------------------- */   
  /* set up serial */
  serial = s;

  /* set up chatAt server callbacks */
  at_srv.set_io_callback({
    .callback_io_read = [this](auto buf, auto len) {
      if (!serial->available()) {
        yield();
        return (unsigned int)0;
      }
      return serial->readBytes(buf, min((unsigned int)serial->available(), len));
    },
    .callback_io_write = [this](auto buf, auto len) {
      return serial->write(buf, len);
    },
  });

  at_srv.set_command_callback([this](chAT::Server & srv, const std::string & command) {
    auto it = command_table.find(command);

    if (it == command_table.end()) {
      return chAT::CommandStatus::ERROR;
    } 
    else {
      return it->second(srv, srv.parser());
    }
  });

  /*  SET UP COMMAND TABLE */ 
  add_cmds_ci();
}
