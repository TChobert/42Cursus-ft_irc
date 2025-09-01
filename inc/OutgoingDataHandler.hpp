#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>

#include "Client.hpp"

enum sendingStatus {
	ERROR,
	REMAINING_DATA,
	EVERYTHING_SENT
};

class OutgoingDataHandler {

	private:

	void updateClientEpoll(Client& client, bool needsOutput, int& epollFd);

	public:

	OutgoingDataHandler(void);
	~OutgoingDataHandler(void);

	sendingStatus handle(Client& client, int& epollFd);
};
