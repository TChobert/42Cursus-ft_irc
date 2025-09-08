#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cerrno>

#include "Client.hpp"
#include "Channel.hpp"

struct modeChange {
	char mode;
	bool adding;
};

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
	void channelsAndKeysJoinAttempt(Client& client, std::vector<std::string>& channelsNames, const std::vector<std::string>& keys, std::map<std::string, Channel*>& channels);
	void createChannel(Client& client, std::string& channelName, std::map<std::string, Channel*>& channels, const std::string& key);
	bool checkChannelExistence(std::string& chanName, std::map<std::string, Channel*>& channels);
	void handleSingleClientKicking(Command& command, Client& requester, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void handleMultipleClientsKicking(std::vector<std::string>& params, Client& requester, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	Client *getClientByName(const std::string& target, std::map<int, Client>& clients) const;
	std::vector<modeChange> getModeFlags(const std::string& modesStr);
	void applyModeFlags(Client& client, std::map<int, Client>& clients, std::vector<modeChange>& flags, std::vector<std::string>& params, Channel *chan, size_t& paramIndex);
	bool setNewChanKey(Client& client, std::string& param, Channel *chan);
	bool setChanOperator(Client& requester, std::map<int, Client>& clients, modeChange& currentMode, std::string& param, Channel *channel);
	bool setChanNewUserLimit(Client& client, std::string& param, Channel *channel);
	void displayChannelParameters(std::string& channelName, Client& requester, std::map<std::string, Channel*> channels);
	void printClients(std::map<int, Client>& clients) const;
	void printChannels(std::map<std::string, Channel*> channels) const;

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
	void commandQuit(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandKick(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandInvite(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandTopic(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandMode(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
	void commandMyServer(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels);
};
