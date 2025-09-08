#include "CommandsExecutor.hpp"

CommandsExecutor::CommandsExecutor(const std::string& serverPswd) : _serverPswd(serverPswd), _commandsProcesses(serverPswd) {}

CommandsExecutor::~CommandsExecutor(void) {}

std::string CommandsExecutor::commandTypeToString(const commandType& type) const {
	switch (type) {
		case CMD_CAP:
			return "CAP";
		case CMD_PING:
			return "PING";
		case CMD_PASS:
			return "PASS";
		case CMD_NICK:
			return "NICK";
		case CMD_USER:
			return "USER";
		case CMD_JOIN:
			return "JOIN";
		case CMD_PRIVMSG:
			return "PRIVMSG";
		case CMD_KICK:
			return "KICK";
		case CMD_INVITE:
			return "INVITE";
		case CMD_TOPIC:
			return "TOPIC";
		case CMD_MODE:
			return "MODE";
		case CMD_MYSERVER:
			return "MYSERVER";
		case CMD_DISPLAYCMDS:
			return "DISPLAYCMDS";
		case CMD_QUIT:
			return "QUIT";
		default:
			return "UNKNOWN";
	}
}

void CommandsExecutor::displayCommand(Client& client, Command& command) const {

	std::cout << BRIGHT_BLUE << "------- COMMAND RECEIVED -------" << RESET << std::endl;

	std::ostringstream who;
	who << client.getNickname() << " (fd " << client.getFd() << ")";
	std::cout << BRIGHT_GREEN << "From: " << RESET << who.str() << std::endl;

	std::string type = commandTypeToString(command.getCommandType());
	std::cout << BRIGHT_GREEN << "Type: " << RESET << type
		<< " [" << command.getCommand() << "]" << std::endl;

	std::ostringstream params;
	const std::vector<std::string>& p = command.getParams();
	for (size_t i = 0; i < p.size(); ++i) {
		if (i != 0) params << ", ";
		params << p[i];
	}
	std::cout << BRIGHT_CYAN << "Params: " << RESET << (p.empty() ? "-" : params.str()) << std::endl;

	std::cout << BRIGHT_MAGENTA << "Trailing: " << RESET
		<< (command.getTrailing().empty() ? "-" : command.getTrailing()) << std::endl;

	std::cout << BRIGHT_BLUE << "--------------------------------" << RESET << std::endl;
}

void CommandsExecutor::executeCurrentCommand(Client& client, Command& currentCommand, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	if (client.getCommandsDisplaying()) {
		displayCommand(client, currentCommand);
	}
	CommandsProcessingStore::CommandProcessPtr commandExecution = _commandsProcesses.getCommandProcess(currentCommand);
	if (!commandExecution)
		return;

	(_commandsProcesses.*commandExecution)(currentCommand, client, clients, channels);
}

const std::string& CommandsExecutor::getServerPswd(void) {
	return (_serverPswd);
}

void CommandsExecutor::execute(Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::vector<Command>& commands = client.getCommands();

	for (std::vector<Command>::iterator it = commands.begin(); it != commands.end(); ++it) {

		executeCurrentCommand(client, *it, clients, channels);
	}
	commands.clear();
}
