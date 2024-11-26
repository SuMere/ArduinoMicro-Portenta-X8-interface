#ifndef __CMD_HANDLER_H__
#define __CMD_HANDLER_H__

#include "chAT.hpp"

class CAtHandler;

class CmdHandler {
	private:
		virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }
		virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&) { return chAT::CommandStatus::ERROR; }

		virtual ~CmdHandler() = 0;

		CAtHandler *m_parent;
	public:
		CmdHandler(CAtHandler *parent) : m_parent(parent) {}
};

#endif /* __CMD_HANDLER_H__ */
