#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>

#include "Client.hpp"

enum sendingStatus {
	ERROR,
	REMAINING_DATA,
	EVERYTHING_SENT
};

class OutgoingDataHandler {

	private:

	public:

	OutgoingDataHandler(void);
	~OutgoingDataHandler(void);

	sendingStatus handle(Client& client);
};
