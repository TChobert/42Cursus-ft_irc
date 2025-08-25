#include "IncomingDataHandler.hpp"

IncomingDataHandler::IncomingDataHandler(void) {}

IncomingDataHandler::~IncomingDataHandler(void) {}



readStatus IncomingDataHandler::readIncomingData(Client& client) {

	char readContent[1024];

	ssize_t bytesRead = recv(client.getFd(), readContent, sizeof(readContent), 0);
	if (bytesRead > 0) {
		client.appendInput(readContent, bytesRead);
		return (READY_TO_PARSE);
	} else if (bytesRead == EOF) {
		return (DISCONNECTED);
	} else {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return (NOT_READY);
		else {
			perror("recv");
			return (DISCONNECTED);
		}
	}
}

void IncomingDataHandler::handle(Client& client) {

	readStatus status = readIncomingData(client);

	switch (status) {
		case READY_TO_PARSE:
			parseCommands(client);
			break;
		case DISCONNECTED:
			disconnectClient(client);
			break;
		case NOT_READY:
			return ;
	}
}
