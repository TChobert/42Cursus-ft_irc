#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>

#include "Client.hpp"

#define EOF 0
#define SPACE ' '

enum readStatus {

	READY_TO_PARSE,
	DISCONNECTED,
	NOT_READY
};

enum commandParseStatus {

	IN_PROGRESS,
	UNCOMPLETE_COMMAND,
	COMPLETE_COMMAND,
	KNOWN_COMMAND,
	UNKNOWN_COMMAND
};

class IncomingDataHandler {

	private:

	static const char * CRLF;
	readStatus readIncomingData(Client& client);
	void parseCommands(Client& client);
	void getCommandPrefix(std::string& line, Command& currentCommand, size_t& index);
	void getCommand(std::string& line, Command& currentCommand, size_t& index, commandParseStatus& status);
	commandParseStatus defineCommandType(Command& currentCommand, const std::string& commandKey);
	commandParseStatus ensureCommandIsComplete(commandType type);
	void getParamsAndTrailing(std::string& line, Command& currentCommand, size_t& index, commandParseStatus& status);
	void addCommandToList(Client& client, Command& command);
	void trimSpaces(std::string& str);

	public:

	IncomingDataHandler(void);
	~IncomingDataHandler(void);

	ExecutionStatus handle(Client& client);
};
