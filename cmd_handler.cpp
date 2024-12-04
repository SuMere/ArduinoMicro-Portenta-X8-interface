#include "cmd_handler.h"
#include "at_handler.h"

CmdHandler::CmdHandler(std::string cmd)
{
    at_handler().registerCommands(cmd, this);
}

chAT::CommandStatus CmdHandler::write_ok_message(chAT::Server &srv, std::string message)
{
    srv.write_response_prompt();
    if(message.length() > 0)
    {
        srv.write_str(message.c_str());
    }
    srv.write_str(" - ");
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus CmdHandler::write_error_message(chAT::Server &srv, std::string message)
{
    srv.write_response_prompt();
    if(message.length() > 0)
    {
        srv.write_str(message.c_str());
    }
    srv.write_str(" - ");
    return chAT::CommandStatus::ERROR;
}