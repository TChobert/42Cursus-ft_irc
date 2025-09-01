#include "CommandsProcessingStore.hpp"

CommandsProcessingStore::CommandsProcessingStore(const std::string& serverPswd) : _serverPswd(serverPswd) {}

CommandsProcessingStore::~CommandsProcessingStore(void) {}

const std::string CommandsProcessingStore::_validNickChars = "[]\\`^{}|";

std::string CommandsProcessingStore::strToLowerRFC(std::string& str) {

	std::string result = str;

	for (size_t i = 0; i < result.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(result[i]);

		if (c >= 'A' && c <= 'Z')
			result[i] = static_cast<char>(c + 32);
		else if (c == '[')
			result[i] = '{';
		else if (c == ']')
			result[i] = '}';
		else if (c == '\\')
			result[i] = '|';
		else if (c == '^')
			result[i] = '~';
	}
	return (result);
}

std::vector<std::string> CommandsProcessingStore::split(const std::string& str, const std::string& delimiter) {

	std::vector<std::string> split;
	std::string temp = str;
	size_t pos;

	while ((pos = temp.find(delimiter)) != std::string::npos) {
		std::string token = temp.substr(0, pos);
		split.push_back(token);
		temp.erase(0, pos + delimiter.length());
	}
	if (!temp.empty()) {
		split.push_back(temp);
	}
	return (split);
}

std::string CommandsProcessingStore::getReplyTarget(const Client& client) const {
	return (client.isRegistered() ? client.getNickname() : "*");
}

const std::string& CommandsProcessingStore::getServerPswd(void) const {
	return (_serverPswd);
}

bool CommandsProcessingStore::isChannel(const std::string& target) const {
	return (target[0] == '#');
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

void CommandsProcessingStore::unknownCommand(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)channels;
	std::cerr << "[ERROR] Unknown or unregistered command: " << command.getCommand() << std::endl;
	client.enqueueOutput(":myserver 421 " + client.getPrefix() + " " + command.getCommand() + " :Unknown command");
}

void CommandsProcessingStore::commandCap(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)command;
	(void)channels;
	client.enqueueOutput(":myserver CAP * LS");
}

void CommandsProcessingStore::commandPing(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)channels;
	std::string token;
	if (!command.getParams().empty())
		token = command.getParams()[0];
	else if (!command.getTrailing().empty())
		token = command.getTrailing();
	else {
		client.enqueueOutput(":myserver 409 " + getReplyTarget(client) + " :No origin specified");
		return ;
	 }

	client.enqueueOutput("PONG " + token);
}

void CommandsProcessingStore::commandPass(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)channels;

	std::cout << "Command PASS" <<std::endl;
	if (client.authProcessStatus._passValidated == true) {
		 client.enqueueOutput(":myserver 462 * :You may not reregister");
		return ;
	}
	std::string pswd = command.getParam(0);
	if (pswd.empty()) {
		 client.enqueueOutput(":myserver 461 " + getReplyTarget(client) + " PASS :Not enough parameters");
		return ;
	}
	if (pswd == getServerPswd()) {
		client.authProcessStatus._passValidated = true;
	} else {
		client.enqueueOutput(":myserver 464 " + getReplyTarget(client) + " :Password incorrect");
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
		if (it->second.getNormalizedRfcNickname() == lowerNick)
			return (true);
	}
	return (false);
}

void CommandsProcessingStore::commandNick(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)channels;
	std::cout << "Command NICK" <<std::endl;
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

void CommandsProcessingStore::commandUser(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)channels;

	std::cout << "Command USER" <<std::endl;
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

bool CommandsProcessingStore::privmsgTargetCheckup(const Client& sender, Client& target, const std::string& targetName, const std::string& message) {

	if (target.isRegistered()) {
		std::string fullMsg = sender.getPrefix() + " PRIVMSG " + targetName + " :" + message;
		target.enqueueOutput(fullMsg);
		return (true);
	}
	return (false);
}

void CommandsProcessingStore::privmsgToClient(Client& sender, std::string& target, std::map<int, Client>& clients, std::string message) {

	for (std::map<int, Client>::iterator cit = clients.begin(); cit != clients.end(); ++ cit) {
		if (cit->second.getNormalizedRfcNickname() == target) {
			if (privmsgTargetCheckup(sender, cit->second, target, message))
				return ;
		}
	}
	sender.enqueueOutput(":myserver 401 " + getReplyTarget(sender) + " " + target + " :No such nick/channel");
}

void CommandsProcessingStore::privmsgToChannel(Client& sender, std::string& target, std::map<std::string, Channel*>& channels, std::string message) {

	std::cout << RED << "TARGET IS : " << target << std::endl << RESET;
	if (target.empty()) {
		std::cout << "ONE !" << std::endl;
		sender.enqueueOutput(":myserver 401 " + getReplyTarget(sender) + " " + target + " :No such nick/channel");
		return ;
	}
	std::map<std::string, Channel*>::iterator it = channels.find(target);
	if(it != channels.end()) {
		std::cout << "TWO !" << std::endl;
		if (it->second->isMember(sender.getNormalizedRfcNickname())) {
			std::string fullMsg = ":" + sender.getPrefix() + " PRIVMSG " + target + " :" + message;
			it->second->broadcastMsg(sender.getNormalizedRfcNickname(), fullMsg);
		} else {
			std::cout << "THREE !" << std::endl;
			sender.enqueueOutput(":myserver 404 " + getReplyTarget(sender) + " " + target + " :Cannot send to channel (not a member)");
		}
		return ;
	}
	sender.enqueueOutput(":myserver 401 " + getReplyTarget(sender) + " " + target + " :No such nick/channel");
}

void CommandsProcessingStore::commandPrivmsg(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

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

		std::string target = strToLowerRFC(*it);

		if (isChannel(*it)) {
			privmsgToChannel(client, target, channels, message);
		} else {
			privmsgToClient(client, target, clients, message);
		}
	}
}

void CommandsProcessingStore::createChannel(Client& client, std::string& channelName, std::map<std::string, Channel*>& channels, const std::string& key) {

	std::string normName = strToLowerRFC(channelName);

	std::map<std::string, Channel*>::iterator it = channels.find(normName);
	Channel* chan = NULL;

	if (it != channels.end()) {
		chan = it->second;
	} else {
		chan = new Channel(channelName);
		if (!key.empty())
			chan->setKey(key);
		channels[normName] = chan;
	}
	chan->addMember(&client);
	chan->addOperator(client);
	std::string welcomeMsg = ":myserver NOTICE " + client.getNickname() + " :Welcome to " + channelName + "!";
	client.enqueueOutput(welcomeMsg);
}

void CommandsProcessingStore::channelsAndKeysJoinAttempt(Client& client, std::vector<std::string>& channelsNames, const std::vector<std::string>& keys, std::map<std::string, Channel*>& channels) {

	for (std::vector<std::string>::iterator it = channelsNames.begin(); it != channelsNames.end(); ++it) {

		size_t index = it - channelsNames.begin();
		std::string currentChanName = strToLowerRFC(*it);
		std::map<std::string, Channel*>::iterator pos = channels.find(currentChanName);

		if (pos != channels.end()) {
			Channel* chan = pos->second;

			if (chan->isMember(client.getNormalizedRfcNickname())) {
				client.enqueueOutput(":myserver 443 " + client.getNickname() + " " + *it + " :is already on channel");
				continue ;
			}
			if (chan->isKeyProtected()) {
				if (index >= keys.size() || !chan->checkKey(keys.at(index))) {
					client.enqueueOutput(":myserver 475 " + client.getNickname() + " " + *it + " :Cannot join channel (+k)");
					continue ;
				}
			}
			chan->addMember(&client);
			std::string joinNotice = ":myserver NOTICE " + client.getNickname() + " :Welcome to " + chan->getChanName() + "!";
			client.enqueueOutput(joinNotice);
		}  else {
			std::string key = (index < keys.size()) ? keys.at(index) : "";
			createChannel(client, *it, channels, key);
		}
	}
}

void CommandsProcessingStore::channelsJoinAttempt(Client& client, std::vector<std::string>& channelsNames, std::map<std::string, Channel*>& channels) {

	for (std::vector<std::string>::iterator it = channelsNames.begin(); it != channelsNames.end(); ++it) {
		std::string currentChanName = strToLowerRFC(*it);
		std::map<std::string, Channel*>::iterator pos = channels.find(currentChanName);

		if (pos != channels.end()) {
			Channel* chan = pos->second;
			if (chan->isMember(client.getNormalizedRfcNickname())) {
				client.enqueueOutput(":myserver 443 " + client.getNickname() + " " + *it + " :is already on channel");
				continue;
			}
			if (chan->isKeyProtected()) {
				client.enqueueOutput(":myserver 475 " + client.getNickname() + " " + *it + " :Cannot join channel (+k)");
				continue;
			}
			chan->addMember(&client);
			std::string joinNotice = ":myserver NOTICE " + client.getNickname() + " :Welcome to " + chan->getChanName() + "!";
			client.enqueueOutput(joinNotice);
		} else {
			createChannel(client, *it, channels, "");
		}
	}
}


void CommandsProcessingStore::joinChannels(std::vector<std::string> channelsAndKeys, Client& client, std::map<std::string, Channel*>& channels) {

	std::vector<std::string> channelsNames = split(channelsAndKeys.at(0), ",");

	if (channelsAndKeys.size() > 1) {
		std::vector<std::string> keys = split(channelsAndKeys.at(1), ",");
		channelsAndKeysJoinAttempt(client, channelsNames, keys, channels);
	} else {
		channelsJoinAttempt(client, channelsNames, channels);
	}
}

void CommandsProcessingStore::commandJoin(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	std::vector<std::string> params = command.getParams();
	if (params.empty() || params.size() > 2) {
		client.enqueueOutput(":myserver 461 " + client.getNickname() + " JOIN :Not enough parameters");
		return ;
	}
	joinChannels(params, client, channels);
}

void CommandsProcessingStore::commandQuit(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	(void)channels;
	std::string message = command.getTrailing();
	std::string quitMessage;

	if (!message.empty()) {
		quitMessage = ":" + client.getPrefix() + " QUIT :" + message;
	} else {
		quitMessage = ":" + client.getPrefix() + " QUIT :Client Quit";
	}
	client.setQuitMessage(quitMessage);
	client.setDisconnectionStatus();
}

CommandsProcessingStore::CommandProcessPtr CommandsProcessingStore::getCommandProcess(Command& command) {

	std::cout << "FUNCTION GET COMMAND PROCESS" << std::endl;
	switch(command.getCommandType()) {
		case CMD_CAP:
			return (&CommandsProcessingStore::commandCap);
		case CMD_PING:
			return (&CommandsProcessingStore::commandPing);
		case CMD_PASS:
			return (&CommandsProcessingStore::commandPass);
		case CMD_NICK:
			return (&CommandsProcessingStore::commandNick);
		case CMD_USER:
			return (&CommandsProcessingStore::commandUser);
		case CMD_JOIN:
			return (&CommandsProcessingStore::commandJoin);
		case CMD_PRIVMSG:
			return (&CommandsProcessingStore::commandPrivmsg);
		case CMD_QUIT:
			return (&CommandsProcessingStore::commandQuit);
		case CMD_UNKNOWN:
			return (&CommandsProcessingStore::unknownCommand);
	}
}

// std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
//     std::vector<std::string> tokens;
//     size_t pos = 0;
//     std::string token;
//     while ((pos = s.find(delimiter)) != std::string::npos) {
//         token = s.substr(0, pos);
//         tokens.push_back(token);
//         s.erase(0, pos + delimiter.length());
//     }
//     tokens.push_back(s);

//     return tokens;
// }
