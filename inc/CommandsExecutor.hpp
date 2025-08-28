#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "Client.hpp"
#include "CommandsProcessingStore.hpp"

class CommandsExecutor {

	private:

	std::string _serverPswd;
	CommandsProcessingStore _commandsProcesses;
	void executeCurrentCommand(Client& client, Command& currentCommand, std::map<int, Client>& clients);

	public:

	CommandsExecutor(const std::string& serverPswd);
	~CommandsExecutor(void);

	const std::string& getServerPswd(void);
	void execute(Client& client, std::map<int, Client>& clients);
};
