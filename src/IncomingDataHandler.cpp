#include "IncomingDataHandler.hpp"

IncomingDataHandler::IncomingDataHandler(void) {}

IncomingDataHandler::~IncomingDataHandler(void) {}

const char * IncomingDataHandler::CRLF = "\r\n";

void IncomingDataHandler::getCommandPrefix(std::string& line, Command& currentCommand, size_t& index) {

	if (line[0] != ':')
		return ;
	size_t space = line.find(SPACE);
	if (space != std::string::npos) {
		currentCommand.setPrefix(line.substr(1, space - 1));
		index += space + 1;
	} else {
		return ;
	}
}

void IncomingDataHandler::defineCommandType(Command& currentCommand, const std::string& commandKey) {

	if (currentCommand._typesDictionary.count(commandKey)) {
		currentCommand.setCommandType(currentCommand._typesDictionary[commandKey]);
	} else {
		currentCommand.setCommandType(CMD_UNKNOWN);
	}
}

void IncomingDataHandler::getCommand(std::string& line, Command& currentCommand, size_t& index) {

	std::string command;

	size_t space = line.find(SPACE, index);
	if (space == std::string::npos) {
		command = line. substr(index);
		defineCommandType(command);
		
	}
}

void IncomingDataHandler::parseCommands(Client& client) {

	std::string& buffer = client.getInputBuffer();
	size_t pos;

	while ((pos = buffer.find(CRLF)) != std::string::npos) {

		std::string line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);

		if (line.empty())
			continue;

		Command currentCommand;
		size_t index = 0;

		getCommandPrefix(line, currentCommand, index);
		getCommand(line, currentCommand, index);
		getParamsAndTrailing(line, currentCommand, index);
		addCommand(currentCommand, client);
	}
}

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
