#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "Client.hpp"

#define EOF 0

enum readStatus {

	READY_TO_PARSE,
	DISCONNECTED,
	NOT_READY
};

class IncomingDataHandler {

	private:

	readStatus readIncomingData(Client& client);
	void parseCommands(Client& client);

	public:

	IncomingDataHandler(void);
	~IncomingDataHandler(void);

	void handle(Client& client);
};
