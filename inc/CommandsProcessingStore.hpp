#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

class CommandsProcessingStore {

	private:

	const std::string& _serverPswd;
	static const std::string _validNickChars;

	std::vector<std::string> split(const std::string& str, const std::string& delimiter);
	bool checkNicknameValidity(const std::string& nickname);
	bool isValidChar(const char c) const;
	bool isAlreadyInUse(std::string& nickname, const std::map<int, Client>& clients) const;
	bool isChannel(const std::string& target) const;
	void privmsgToChannel(Client& sender, std::string& target, std::map<std::string, Channel*>& channels, std::string message);
	void privmsgToClient(Client& sender, std::string& target, std::map<int, Client>& clients, std::string message);
	std::string getReplyTarget(const Client& client) const;
	void sendWelcomeMessages(Client& client);
	std::string strToLowerRFC(std::string& str);
	bool privmsgTargetCheckup(const Client& sender, Client& target, const std::string& targetName, const std::string& message);
	void joinChannels(std::vector<std::string> channelsAndKeys, Client& client, std::map<std::string, Channel*>& channels);
	void channelsJoinAttempt(Client& client, std::vector<std::string>& channelsNames, std::map<std::string, Channel*>& channels);
	void createChannel(Client& client, std::string& channelName, std::map<std::string, Channel*>& channels, const std::string& key);

	public:

	typedef void (CommandsProcessingStore::*CommandProcessPtr)(Command& command, Client&, std::map<int, Client>&, std::map<std::string, Channel*>& channels);

	CommandsProcessingStore(const std::string& serverPswd);
	~CommandsProcessingStore(void);

	CommandProcessPtr getCommandProcess(Command& command);
	const std::string& getServerPswd(void) const;
	void unknownCommand(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandCap(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandPing(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandPass(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandNick(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandUser(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandPrivmsg(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandJoin(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
};
