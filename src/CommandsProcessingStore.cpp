#include "CommandsProcessingStore.hpp"

CommandsProcessingStore::CommandsProcessingStore(const std::string& serverPswd) : _serverPswd(serverPswd) {}

CommandsProcessingStore::~CommandsProcessingStore(void) {}

const std::string CommandsProcessingStore::_validFirstChars = "[]\\`^{}|";

const std::string& CommandsProcessingStore::getServerPswd(void) const {
	return (_serverPswd);
}

bool CommandsProcessingStore::checkNicknameValidity(const std::string& nickname) {

	char firstChar = nickname[0];

	bool isLetter = (firstChar >= 'A' && firstChar <= 'Z') || (firstChar >= 'a' && firstChar <= 'z');
	if (!isLetter && _validFirstChars.find_first_of(firstChar) == std::string::npos) {
		return (false);
	}
}

void CommandsProcessingStore::commandPass(Command& command, Client& client, std::map<int, Client>& clients) {

	if (client.authProcessStatus._passValidated == true) {
		 client.enqueueOutput(":myserver 462 " + client.getNickname() + " :You may not reregister");
		return ;
	}
	std::string pswd = command.getParam(0);
	if (pswd.empty()) {
		 client.enqueueOutput(":myserver 461 " + client.getNickname() + " PASS :Not enough parameters");
		return ;
	}
	if (pswd == getServerPswd()) {
		client.authProcessStatus._passValidated = true;
	} else {
		client.enqueueOutput(":myserver 464 " + client.getNickname() + " :Password incorrect");
	}
}

void CommandsProcessingStore::commandNick(Command& command, Client& client, std::map<int, Client>& clients) {

	if (!client.authProcessStatus._passValidated) {
		client.enqueueOutput(":myserver 451 NICK :You have not registered");
		return ;
	}
	std::string nickname = command.getParam(0);
	if (nickname.empty()) {
		client.enqueueOutput(":myserver 461 " + client.getNickname() + " NICK :not enough parameters");
		return ;
	}
	if (!checkNicknameValidity(nickname)) {
		client.enqueueOutput(":myserver 432 " + client.getNickname() + " " + nickname + " :Erroneous nickname");
		return ;
	}
}

CommandsProcessingStore::CommandProcessPtr CommandsProcessingStore::getCommandProcess(Command& command) {

	switch(command.getCommandType()) {
		case CMD_PASS:
			return (&CommandsProcessingStore::commandPass);
		case CMD_NICK:
			return (&CommandsProcessingStore::commandNick);
		// case CMD_USER:
		// 	return (&CommandUser);
	}
}
