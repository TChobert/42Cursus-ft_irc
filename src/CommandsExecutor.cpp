#include "CommandsExecutor.hpp"

CommandsExecutor::CommandsExecutor(const std::string& serverPswd) : _serverPswd(serverPswd), _commandsProcesses(serverPswd) {}

CommandsExecutor::~CommandsExecutor(void) {}

void CommandsExecutor::executeCurrentCommand(Client& client, Command& currentCommand, std::map<int, Client>& clients) {

	CommandsProcessingStore::CommandProcessPtr commandExecution = _commandsProcesses.getCommandProcess(currentCommand);
	(_commandsProcesses.*commandExecution)(currentCommand, client, clients);
}

const std::string& CommandsExecutor::getServerPswd(void) {
	return (_serverPswd);
}

void CommandsExecutor::execute(Client& client, std::map<int, Client>& clients) {

	std::vector<Command>& commands = client.getCommands();

	for (std::vector<Command>::iterator it = commands.begin(); it != commands.end(); ++it) {

		executeCurrentCommand(client, *it, clients);
	}
	commands.clear();
}
