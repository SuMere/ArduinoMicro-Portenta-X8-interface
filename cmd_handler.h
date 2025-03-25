#ifndef __CMD_HANDLER_H__
#define __CMD_HANDLER_H__

#include "chAT.hpp"
using namespace SudoMaker;

class CAtHandler;

/**
 * @class CmdHandler
 * @brief Base class for handling AT commands.
 * Provides virtual methods for handling different command modes (read, write, test, run, etc.).
 */
class CmdHandler {
    friend class CAtHandler;

private:
    /**
     * @brief Handles the "read" command mode.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_read(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

    /**
     * @brief Handles the "write" command mode.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_write(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

    /**
     * @brief Handles the "test" command mode. In this FW used as helper command for user interaction.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_test(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

    /**
     * @brief Handles the "run" command mode.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_run(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

    /**
     * @brief Handles the "read configuration" command mode.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_cfg_read(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

    /**
     * @brief Handles the "write configuration" command mode.
     * @param srv Reference to the chAT server.
     * @param parser Reference to the ATParser instance.
     * @return Command status indicating the result of the operation.
     */
    virtual chAT::CommandStatus handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser) { return chAT::CommandStatus::ERROR; }

protected:
    /**
     * @brief Sends an "OK" response message to the server.
     * @param srv Reference to the chAT server.
     * @param message The message to include in the response.
     * @return Command status indicating success (OK).
     */
    chAT::CommandStatus write_ok_message(chAT::Server &srv, std::string message);

    /**
     * @brief Sends an "ERROR" response message to the server.
     * @param srv Reference to the chAT server.
     * @param message The message to include in the response.
     * @return Command status indicating failure (ERROR).
     */
    chAT::CommandStatus write_error_message(chAT::Server &srv, std::string message);

public:
    /**
     * @brief Deleted default constructor to enforce the use of the parameterized constructor.
     */
    CmdHandler() = delete;

    /**
     * @brief Constructs a CmdHandler and registers the command with the AT handler.
     * @param cmd The command string to register.
     */
    CmdHandler(std::string cmd);

    /**
     * @brief Virtual destructor for the CmdHandler class.
     */
    virtual ~CmdHandler() {}
};

#endif /* __CMD_HANDLER_H__ */
