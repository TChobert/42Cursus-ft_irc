#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "Client.hpp"
#include "CommandsProcessingStore.hpp"

class CommandsExecutor {

	private:

	CommandsProcessingStore _commandsProcesses;
	void executeCurrentCommand(Command currentCommand, std::map<int, Client>& clients);

	public:

	CommandsExecutor(void);
	~CommandsExecutor(void);

	void execute(Client& client, std::map<int, Client>& clients);
};
