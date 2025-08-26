#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Client.hpp"

class CommandsProcessingStore {

	private:

	const std::string& _serverPswd;
	static const std::string _validFirstChars;

	bool checkNicknameValidity(const std::string& nickname);

	public:

	typedef void (CommandsProcessingStore::*CommandProcessPtr)(Command& command, Client&, std::map<int, Client>&);

	CommandsProcessingStore(const std::string& serverPswd);
	~CommandsProcessingStore(void);

	CommandProcessPtr getCommandProcess(Command& command);
	const std::string& getServerPswd(void) const;
	void commandPass(Command& command, Client& client, std::map<int, Client>& clients);
	void commandNick(Command& command, Client& client, std::map<int, Client>& clients);
};
