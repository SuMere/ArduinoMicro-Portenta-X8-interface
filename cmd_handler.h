#ifndef __CMD_HANDLER_H__
#define __CMD_HANDLER_H__

#include "chAT.hpp"
using namespace SudoMaker;

class CAtHandler;

class CmdHandler {
	friend class CAtHandler;
	private:
		virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }

	public:
		CmdHandler() = delete;
		CmdHandler(std::string cmd);
		virtual ~CmdHandler() {}
};

#endif /* __CMD_HANDLER_H__ */
