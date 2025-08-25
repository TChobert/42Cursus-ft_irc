#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "Client.hpp"

#define EOF 0
#define SPACE ' '

enum readStatus {

	READY_TO_PARSE,
	DISCONNECTED,
	NOT_READY
};

class IncomingDataHandler {

	private:

	static const char * CRLF;
	readStatus readIncomingData(Client& client);
	void parseCommands(Client& client);
	void getCommandPrefix(std::string& line, Command& currentCommand, size_t& index);
	void getCommand(std::string& line, Command& currentCommand, size_t& index);
	void defineCommandType(Command& currentCommand, const std::string& commandKey);

	public:

	IncomingDataHandler(void);
	~IncomingDataHandler(void);

	void handle(Client& client);
};
