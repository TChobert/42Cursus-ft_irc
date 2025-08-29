#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Client.hpp"

class CommandsProcessingStore {

	private:

	const std::string& _serverPswd;
	static const std::string _validNickChars;

	bool checkNicknameValidity(const std::string& nickname);
	bool isValidChar(const char c) const;
	bool isAlreadyInUse(std::string& nickname, const std::map<int, Client>& clients) const;
	std::string getReplyTarget(const Client& client) const;
	void sendWelcomeMessages(Client& client);
	std::string strToLower(std::string& str);
	void privmsgTargetCheckup(const Client& sender, Client& target, const std::string& targetName, const std::string& message);

	public:

	typedef void (CommandsProcessingStore::*CommandProcessPtr)(Command& command, Client&, std::map<int, Client>&);

	CommandsProcessingStore(const std::string& serverPswd);
	~CommandsProcessingStore(void);

	CommandProcessPtr getCommandProcess(Command& command);
	const std::string& getServerPswd(void) const;
	void unknownCommand(Command& command, Client& client, std::map<int, Client>& clients);
	void commandCap(Command& command, Client& client, std::map<int, Client>& clients);
	void commandPass(Command& command, Client& client, std::map<int, Client>& clients);
	void commandNick(Command& command, Client& client, std::map<int, Client>& clients);
	void commandUser(Command& command, Client& client, std::map<int, Client>& clients);
	void commandPrivmsg(Command& command, Client& client, std::map<int, Client>& clients);
};
