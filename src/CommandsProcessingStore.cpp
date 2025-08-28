#include "CommandsProcessingStore.hpp"

CommandsProcessingStore::CommandsProcessingStore(const std::string& serverPswd) : _serverPswd(serverPswd) {}

CommandsProcessingStore::~CommandsProcessingStore(void) {}

const std::string CommandsProcessingStore::_validNickChars = "[]\\`^{}|";

std::string CommandsProcessingStore::strToLower(std::string& str) {

	std::string result = str;

	for (size_t i = 0; i < result.size(); ++i) {
		result[i] = std::tolower(result[i]);
	}
	return (result);
}

std::string CommandsProcessingStore::getReplyTarget(const Client& client) const {
	return (client.isRegistered() ? client.getNickname() : "*");
}

const std::string& CommandsProcessingStore::getServerPswd(void) const {
	return (_serverPswd);
}

bool CommandsProcessingStore::isValidChar(char c) const {
	return std::isalnum(static_cast<unsigned char>(c)) || _validNickChars.find(c) != std::string::npos;
}

void CommandsProcessingStore::sendWelcomeMessages(Client& client) {

	const std::string nick = client.getNickname();
	const std::string prefix = client.getPrefix();

	client.enqueueOutput(":myserver 001 " + nick + " :Welcome to the IRC network " + prefix);

	client.enqueueOutput(":myserver 002 " + nick + " :Your host is myserver, running version 1.0");

	client.enqueueOutput(":myserver 003 " + nick + " :This server was created Thu Aug 27 2025");

	client.enqueueOutput(":myserver 004 " + nick + " myserver 1.0 o o");
}

void CommandsProcessingStore::commandPass(Command& command, Client& client, std::map<int, Client>& clients) {

	if (client.authProcessStatus._passValidated == true) {
		 client.enqueueOutput(":myserver 462 * :You may not reregister");
		return ;
	}
	std::string pswd = command.getParam(0);
	if (pswd.empty()) {
		 client.enqueueOutput(":myserver 461 * PASS :Not enough parameters");
		return ;
	}
	if (pswd == getServerPswd()) {
		client.authProcessStatus._passValidated = true;
	} else {
		client.enqueueOutput(":myserver 464 * :Password incorrect");
	}
}

bool CommandsProcessingStore::checkNicknameValidity(const std::string& nickname) {

	char firstChar = nickname[0];

	if (nickname.size() > 9)
		return (false);
	bool isLetter = (firstChar >= 'A' && firstChar <= 'Z') || (firstChar >= 'a' && firstChar <= 'z');
	if (!isLetter && _validNickChars.find_first_of(firstChar) == std::string::npos) {
		return (false);
	}
	for (size_t i = 1; i < nickname.size(); ++i) {
		if (!isValidChar(nickname[i]))
		return (false);
	}
	return (true);
}

bool CommandsProcessingStore::isAlreadyInUse(std::string& nickname, const std::map<int, Client>& clients) const {

	std::string lowerNick = nickname;

	for (size_t i = 0; i < lowerNick.size(); ++i)
		lowerNick[i] = std::tolower(static_cast<unsigned char>(lowerNick[i]));

	for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second.getLowerNickname() == lowerNick)
			return (true);
	}
	return (false);
}

void CommandsProcessingStore::commandNick(Command& command, Client& client, std::map<int, Client>& clients) {

	if (!client.authProcessStatus._passValidated) {
		client.enqueueOutput(":myserver 451 NICK :You have not registered");
		return ;
	}
	std::string nickname = command.getParam(0);
	if (nickname.empty()) {
		client.enqueueOutput(":myserver 461 " + getReplyTarget(client) + " NICK :not enough parameters");
		return ;
	}
	if (!checkNicknameValidity(nickname)) {
		client.enqueueOutput(":myserver 432 " + getReplyTarget(client) + " " + nickname + " :Erroneous nickname");
		return ;
	}
	if (isAlreadyInUse(nickname, clients)) {
		client.enqueueOutput(":myserver 433 " + nickname + " :Nickname is already in use");
		return ;
	}

	client.setNickname(nickname);

	if (!client.isRegistered()) {
		client.authProcessStatus._nickNameSet =true;
		if (client.authProcessStatus.isAuthProcessComplete()) {
			client.setRegistered(true);
			sendWelcomeMessages(client);
		}
	}
}

void CommandsProcessingStore::commandUser(Command& command, Client& client, std::map<int, Client>& clients) {

	if (!client.authProcessStatus._passValidated || !client.authProcessStatus._nickNameSet) {
		client.enqueueOutput(":myserver 451 " + getReplyTarget(client) + " USER :You have not registered");
		return ;
	}
	if (client.isRegistered()) {
		client.enqueueOutput(":myserver 462 " + getReplyTarget(client) + " :You may not reregister");
		return;
	}

	std::vector<std::string> params = command.getParams();
	std::string realName = command.getTrailing();
	if (params.size() < 3 || realName.empty()) {
		client.enqueueOutput(":myserver 461 " + getReplyTarget(client) + " USER :Not enough parameters");
		return ;
	}
	client.setUsername(command.getParam(0));
	client.setRealname(realName);
	client.authProcessStatus._userNameSet = true;
	if (client.authProcessStatus.isAuthProcessComplete()) {
		client.setRegistered(true);
		sendWelcomeMessages(client);
	}
}

void CommandsProcessingStore::privmsgTargetCheckup(const Client& sender, Client& target, const std::string& targetName, const std::string& message) {

	if (target.isAuthentificated()) {
		std::string fullMsg = ":" + sender.getPrefix() + " PRIVMSG " + targetName + " :" + message;
		target.enqueueOutput(fullMsg);
	}
}

void CommandsProcessingStore::commandPrivmsg(Command& command, Client& client, std::map<int, Client>& clients) {

	if (!client.isRegistered()) {
		client.enqueueOutput(":myserver 451 " + getReplyTarget(client) + " PRIVMSG :You have not registered");
		return ;
	}
	std::vector<std::string> targets = command.getParams();
	std::string message = command.getTrailing();

	if (targets.empty()) {
		client.enqueueOutput(":myserver 411 " + getReplyTarget(client) + " :No recipient given (PRIVMSG)");
		return ;
	}
	if (message.empty()) {
		client.enqueueOutput(":myserver 412 " + getReplyTarget(client) + " :No text to send");
		return ;
	}

	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it) {

		std::string target = strToLower(*it);
		bool found = false;

		for (std::map<int, Client>::iterator cit = clients.begin(); cit != clients.end(); ++ cit) {
			if (cit->second.getLowerNickname() == target) {
				found = true;
				privmsgTargetCheckup(client, cit->second, *it, message);
				break ;
			}
		}
		if (!found) {
			client.enqueueOutput(":myserver 401 " + getReplyTarget(client) + " " + *it + " :No such nick/channel");
		}
	}
}

CommandsProcessingStore::CommandProcessPtr CommandsProcessingStore::getCommandProcess(Command& command) {

	switch(command.getCommandType()) {
		case CMD_PASS:
			return (&CommandsProcessingStore::commandPass);
		case CMD_NICK:
			return (&CommandsProcessingStore::commandNick);
		case CMD_USER:
			return (&CommandsProcessingStore::commandUser);
		// case CMD_JOIN:
		// 	return (&CommandsProcessingStore::commandJoin);
		case CMD_PRIVMSG:
			return (&CommandsProcessingStore::commandPrivmsg);
	}
}
