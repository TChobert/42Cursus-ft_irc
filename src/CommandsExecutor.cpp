#include "CommandsExecutor.hpp"

CommandsExecutor::CommandsExecutor(const std::string& serverPswd) : _serverPswd(serverPswd), _commandsProcesses(serverPswd) {}

CommandsExecutor::~CommandsExecutor(void) {}

void CommandsExecutor::executeCurrentCommand(Client& client, Command& currentCommand, std::map<int, Client>& clients) {

	CommandsProcessingStore::CommandProcessPtr commandExecution = _commandsProcesses.getCommandProcess(currentCommand);
	if (!commandExecution) {
    std::cerr << "[ERROR] Unknown or unregistered command: "
              << currentCommand.getCommand() << std::endl;
    // envoyer un message d'erreur au client si nécessaire
    client.enqueueOutput(":myserver 421 " + client.getPrefix() + " " + currentCommand.getCommand() + " :Unknown command");
    return; // ou continue selon ton choix
	}
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
