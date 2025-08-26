#include "CommandsProcessingStore.hpp"

CommandsProcessingStore::CommandsProcessingStore(const std::string& serverPswd) : _serverPswd(serverPswd) {}

CommandsProcessingStore::~CommandsProcessingStore(void) {}

const std::string CommandsProcessingStore::_validNickChars = "[]\\`^{}|";

std::string CommandsProcessingStore::getPrefix(const Client& client) const {
	return (client.isRegistered() ? client.getNickname() : "*");
}

const std::string& CommandsProcessingStore::getServerPswd(void) const {
	return (_serverPswd);
}

bool CommandsProcessingStore::isValidChar(char c) const {
	return std::isalnum(static_cast<unsigned char>(c)) || _validNickChars.find(c) != std::string::npos;
}

void CommandsProcessingStore::sendWelcomeMessages(Client& client) {

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

bool CommandsProcessingStore::isAlreadyInUse(const std::string& nickname, const std::map<int, Client>& clients) const {

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
		client.enqueueOutput(":myserver 461 " + client.getPrefix() + " NICK :not enough parameters");
		return ;
	}
	if (!checkNicknameValidity(nickname)) {
		client.enqueueOutput(":myserver 432 " + client.getPrefix() + " " + nickname + " :Erroneous nickname");
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
		//enqueue invalid order
		return ;
	}
	if (client.isRegistered()) {
		client.enqueueOutput(":myserver 462 " + client.getPrefix() + " :You may not reregister");
		return;
	}

	std::vector<std::string> params = command.getParams();
	std::string realName = command.getTrailing();
	if (params.size() < 3 || realName.empty()) {
		client.enqueueOutput(":myserver 461 " + client.getPrefix() + " USER :Not enough parameters");
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

void CommandsProcessingStore::commandPrivmsg(Command& command, Client& client, std::map<int, Client>& clients) {

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
