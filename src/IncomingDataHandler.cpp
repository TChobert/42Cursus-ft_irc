#include "IncomingDataHandler.hpp"

IncomingDataHandler::IncomingDataHandler(void) {}

IncomingDataHandler::~IncomingDataHandler(void) {}

const char * IncomingDataHandler::CRLF = "\r\n";

void IncomingDataHandler::trimSpaces(std::string& str) {

	while (!str.empty() && str[0] == SPACE)
		str.erase(0, 1);
}

void IncomingDataHandler::getCommandPrefix(std::string& line, Command& currentCommand, size_t& index) {

	trimSpaces(line);
	if (line[0] != ':')
		return ;
	size_t space = line.find(SPACE);
	if (space != std::string::npos) {
		currentCommand.setPrefix(line.substr(1, space - 1));
		index += space;
	} else {
		return ;
	}
}

commandParseStatus IncomingDataHandler::defineCommandType(Command& currentCommand, const std::string& commandKey) {

	if (commandKey == "CAP") {
		currentCommand.setCommandType(CMD_CAP);
		return (KNOWN_COMMAND);
	}
	else if (commandKey == "PING") {
		currentCommand.setCommandType(CMD_PING);
		return (KNOWN_COMMAND);
	}
	else if (commandKey == "PASS") {
		currentCommand.setCommandType(CMD_PASS);
		return (KNOWN_COMMAND);
	} 
	else if (commandKey == "NICK") {
		currentCommand.setCommandType(CMD_NICK);
		return (KNOWN_COMMAND);
	} 
	else if (commandKey == "USER") {
		currentCommand.setCommandType(CMD_USER);
		return (KNOWN_COMMAND);
	}
	else if (commandKey == "JOIN") {
		currentCommand.setCommandType(CMD_JOIN);
		return (KNOWN_COMMAND);
	}
	else if (commandKey == "PRIVMSG") {
		currentCommand.setCommandType(CMD_PRIVMSG);
		return (KNOWN_COMMAND);
	}
	else if (commandKey == "QUIT") {
		currentCommand.setCommandType(CMD_QUIT);
		return (KNOWN_COMMAND);
	}
	else {
		currentCommand.setCommandType(CMD_UNKNOWN);
		return (KNOWN_COMMAND);
	}
}

// commandParseStatus IncomingDataHandler::defineCommandType(Command& currentCommand, const std::string& commandKey) {

// 	if (currentCommand._typesDictionary.count(commandKey)) {
// 		currentCommand.setCommandType(currentCommand._typesDictionary[commandKey]);
// 	} else {
// 		currentCommand.setCommandType(CMD_UNKNOWN);
// 		return (UNKNOWN_COMMAND);
// 	}
// 	return (KNOWN_COMMAND);
// }

commandParseStatus IncomingDataHandler::ensureCommandIsComplete(commandType type) {

  switch (type) {
		case CMD_QUIT:
		case CMD_PING:
			return COMPLETE_COMMAND;
		case CMD_PASS:
		case CMD_NICK:
		case CMD_USER:
		case CMD_JOIN:
		case CMD_PRIVMSG:
	//	case CMD_KICK:
	//	case CMD_INVITE:
	//	case CMD_TOPIC:
	//	case CMD_MODE:
			return UNCOMPLETE_COMMAND;
		default:
			return UNCOMPLETE_COMMAND;
	}
}

void IncomingDataHandler::getCommand(std::string& line, Command& currentCommand, size_t& index, commandParseStatus& status) {

	std::string commandKey;

	trimSpaces(line);
	size_t space = line.find(SPACE, index);

	if (space == std::string::npos) {
		commandKey = line.substr(index);
		status = defineCommandType(currentCommand, commandKey);
		if (status != UNKNOWN_COMMAND)
			status = ensureCommandIsComplete(currentCommand.getCommandType());
	} else {
		commandKey = line.substr(index, space - index);
		defineCommandType(currentCommand, commandKey);
	}
	currentCommand.setCommand(commandKey);
	index += commandKey.size();
}

void splitAndAddParams(std::string params, Command& currentCommand) {

	std::stringstream ss(params);
	std::string currentParam;

	while (ss >> currentParam) {
		currentCommand.addParam(currentParam);
	}
}

void IncomingDataHandler::getParamsAndTrailing(std::string& line, Command& currentCommand, size_t& index) {

	trimSpaces(line);
	size_t trailingOperator = line.find(':', index);

	if (trailingOperator != std::string::npos) {
		std::string paramsPart = line.substr(index, trailingOperator - index);
		splitAndAddParams(paramsPart, currentCommand);

		std::string trailingPart = line.substr(trailingOperator + 1);
		currentCommand.setTrailing(trailingPart);
	} else {
		std::string paramsPart = line.substr(index);
		splitAndAddParams(paramsPart, currentCommand);
	}
}

void IncomingDataHandler::addCommandToList(Client& client, Command& command) {
	command.printCommand();
	client.addCommand(command);
}

void IncomingDataHandler::extractCurrentCommand(std::string& line, Command& currentCommand, size_t& index, commandParseStatus& status) {

	getCommandPrefix(line, currentCommand, index);
	getCommand(line, currentCommand, index, status);
	if (status == IN_PROGRESS) {
		getParamsAndTrailing(line, currentCommand, index);
	}
}

void IncomingDataHandler::saveCurrentCommand(Client& client, Command& command, commandParseStatus& status) {

	if (status == IN_PROGRESS) {
		addCommandToList(client, command);
	} else if (status == COMPLETE_COMMAND) {
		addCommandToList(client, command);
	} else if (status == UNCOMPLETE_COMMAND) {
		client.enqueueOutput(":myserver 461 " + client.getPrefix() + " " + command.getCommand() + " :Not enough parameters");
	} else if (status == UNKNOWN_COMMAND) {
		client.enqueueOutput(":myserver 421 " + client.getPrefix() + " " + command.getCommand() + " :Unknown command");
	}
}

void IncomingDataHandler::parseCommands(Client& client) {

	std::string& buffer = client.getInputBuffer();
	size_t pos;

	while ((pos = buffer.find(CRLF)) != std::string::npos) {

		commandParseStatus status = IN_PROGRESS;

		std::string line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
 		if (line.empty())
			continue;

		Command currentCommand;
		size_t index = 0;

		extractCurrentCommand(line, currentCommand, index, status);
		saveCurrentCommand(client, currentCommand, status);
	}
}

readStatus IncomingDataHandler::readIncomingData(Client& client) {

	char readContent[1024];

	std::cout << "FUNCTION READ INCOMING DATA" << std::endl;
	ssize_t bytesRead = recv(client.getFd(), readContent, sizeof(readContent), 0);
	if (bytesRead > 0) {
		client.appendInput(readContent, bytesRead);
		if (client.isCrlfInInput()) {
			return (READY_TO_PARSE);
		}
		else
			return (NOT_READY);
	} else if (bytesRead == EOF_CLIENT) {
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

ExecutionStatus IncomingDataHandler::receiveDataFromClient(Client& client) {

	std::cout << "FUNCTION HANDLE INCOMING DATA" << std::endl;
	readStatus status = readIncomingData(client);

	switch (status) {
		case READY_TO_PARSE:
			parseCommands(client);
			return (READY_TO_EXECUTE);
		case DISCONNECTED:
			return (DISCONNECTION);
		case NOT_READY:
			return (NO_EXECUTION_NEEDED);
	}
}
