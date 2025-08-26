#include "CommandsExecutor.hpp"

CommandsExecutor::CommandsExecutor(void) {}

CommandsExecutor::~CommandsExecutor(void) {}

void CommandsExecutor::executeCurrentCommand(Command currentCommand, std::map<int, Client> clients) {

	
}

void CommandsExecutor::execute(Client& client, std::map<int, Client> clients) {

	std::vector<Command> commands = client.getCommands();

	for (std::vector<Command>::iterator it = commands.begin(); it != commands.end(); ++it) {

		executeCurrentCommand(*it, clients);
	}
}
