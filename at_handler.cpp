

#include "at_handler.h"

using namespace SudoMaker;

/* -------------------------------------------------------------------------- */
void CAtHandler::run()
/* -------------------------------------------------------------------------- */
{
	at_srv.run();
}

/* -------------------------------------------------------------------------- */
CAtHandler::CAtHandler(HardwareSerial *s)
	: last_server_client_sock(0)
/* -------------------------------------------------------------------------- */
{
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
		if (it != command_table.end()) {
			auto parser = srv.parser();
			switch (parser.cmd_mode) {
			    case chAT::CommandMode::Read:       return it->second->handle_read(srv, parser);
			    case chAT::CommandMode::Write:      return it->second->handle_write(srv, parser);
			}
		} else if (command.find("_CFG") == command.length()-4) {
			auto it = command_table.find(command.substr(0, command.length()-4));
			if (it != command_table.end()) {
				auto parser = srv.parser();
				switch (parser.cmd_mode) {
				    case chAT::CommandMode::Read:       return it->second->handle_cfg_read(srv, parser);
				    case chAT::CommandMode::Write:      return it->second->handle_cfg_write(srv, parser);
				}
			}
		}

		return chAT::CommandStatus::ERROR;
	});
}

/* -------------------------------------------------------------------------- */
void CAtHandler::registerCommands(std::string command, CmdHandler *handler)
/* -------------------------------------------------------------------------- */
{
	command_table[command] = handler;
}
