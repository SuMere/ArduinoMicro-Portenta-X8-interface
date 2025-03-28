#include "at_handler.h"
#include "cmd_handler.h"

using namespace SudoMaker;

/* -------------------------------------------------------------------------- */
CAtHandler::CAtHandler()
/* -------------------------------------------------------------------------- */
	: serial(NULL)
{
}

/* -------------------------------------------------------------------------- */
void CAtHandler::begin(HardwareSerial *s)
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
				case chAT::CommandMode::Test:       return it->second->handle_test(srv, parser);
				case chAT::CommandMode::Run:        return it->second->handle_run(srv, parser);
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
void CAtHandler::run()
/* -------------------------------------------------------------------------- */
{
	if (serial) {
		at_srv.run();
	}
}

/* -------------------------------------------------------------------------- */
void CAtHandler::registerCommands(std::string command, CmdHandler *handler)
/* -------------------------------------------------------------------------- */
{
	command_table[command] = handler;
}

/* -------------------------------------------------------------------------- */
CAtHandler &at_handler()
/* -------------------------------------------------------------------------- */
{
	static CAtHandler at_handler;
	return at_handler;
}
