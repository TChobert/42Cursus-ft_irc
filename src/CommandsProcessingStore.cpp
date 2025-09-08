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
		if (!key.empty()) {
			chan->setKey(key);
		}
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
			if (chan->hasUserLimit()) {
				long userLimit = chan->getUserLimit();
				long chanMembNumber = chan->getMembersNumber();
				if ((chanMembNumber + 1) > userLimit) {
					client.enqueueOutput(":myserver 471 " + client.getNickname() + " " + *it + " :Cannot join channel (+l)");
					continue ;
				}
			}
			chan->addMember(&client);
			std::string joinNotice = ":myserver NOTICE " + client.getNickname() + " :Welcome to " + chan->getChanName() + "!";
			client.enqueueOutput(joinNotice);
		}  else {
			std::string key = (index < keys.size()) ? keys.at(index) : std::string();
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
			if (chan->hasUserLimit()) {
				long userLimit = chan->getUserLimit();
				long chanMembNumber = chan->getMembersNumber();
				if ((chanMembNumber + 1) > userLimit) {
					client.enqueueOutput(":myserver 471 " + client.getNickname() + " " + *it + " :Cannot join channel (+l)");
					continue ;
				}
			}
			if (chan->isInviteOnly()) {
				if (!client.isInvitedTo(chan->getNormalizedChanName())) {
					client.enqueueOutput(":myserver 473 " + client.getNickname() + " " + *it + " :Cannot join channel (+i)");
					continue ;
				}
			}
			chan->addMember(&client);
			std::string joinNotice = ":myserver NOTICE " + client.getNickname() + " :Welcome to " + chan->getChanName() + "!";
			client.enqueueOutput(joinNotice);
		} else {
			createChannel(client, *it, channels, std::string());
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
		quitMessage = client.getPrefix() + " QUIT :" + message;
	} else {
		quitMessage = client.getPrefix() + " QUIT :Client Quit";
	}
	client.setQuitMessage(quitMessage);
	client.setDisconnectionStatus();
}

bool CommandsProcessingStore::checkChannelExistence(std::string& chanName, std::map<std::string, Channel*>& channels) {

	if (chanName[0] != '#' || chanName.empty() || chanName.size() < 2)
		return (false);

	std::string normName = strToLowerRFC(chanName);
	return (channels.count(normName));
}

Client *CommandsProcessingStore::getClientByName(const std::string& wanted, std::map<int, Client>& clients) const {

	Client *found = NULL;
	for (std::map<int, Client>::iterator itClient = clients.begin(); itClient != clients.end(); ++itClient) {
		if (itClient->second.getNormalizedRfcNickname() == wanted) {
			found = &itClient->second;
			break;
		}
	}
	return (found);
}

void CommandsProcessingStore::handleSingleClientKicking(Command& command, Client& requester,  std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::vector<std::string>params = command.getParams();
	std::string chanName = strToLowerRFC(params[0]);
	std::string victimName = strToLowerRFC(params[1]);

	Client *victim = getClientByName(victimName, clients);
	if (!victim) {
		requester.enqueueOutput(":myserver 401 " + requester.getNickname() + " " + victimName + " :No such nick");
		return;
	}

	std::map<std::string, Channel*>::iterator it = channels.find(chanName);
	Channel* chan = it->second;
	if (!chan->isOperator(requester.getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 482 " + requester.getNickname() + " " + chan->getChanName() + " :You're not channel operator");
		return;
	}
	if (!chan->isMember(victim->getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 441 " + requester.getNickname() + " " + victim->getNickname() + " " + chan->getChanName() + " :They aren't on that channel");
		return;
	}
	std::string reason = command.getTrailing();
	if (reason.empty()) reason = "Kicked by " + requester.getNickname();

	std::string kickMsg = ":" + requester.getPrefix() + " KICK " + chan->getChanName() + " " + victim->getNickname() + " :" + reason;
	chan->broadcastMsg("", kickMsg);
	chan->removeMember(*victim);
}

void CommandsProcessingStore::handleMultipleClientsKicking(std::vector<std::string>& params, Client& requester, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::string chanName = params[0];
	std::vector<std::string> victims = split(params[1], ",");
	for (size_t i = 0; i < victims.size(); ++i) {

		Command fakeCommand;
		std::vector<std::string> fakeParams;
		fakeParams.push_back(chanName);
		fakeParams.push_back(victims[i]);
		fakeCommand.setParams(fakeParams);
		fakeCommand.setTrailing(params.size() > 2 ? params[2] : "");
		handleSingleClientKicking(fakeCommand, requester, clients, channels);
	}
}

void CommandsProcessingStore::commandKick(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::vector<std::string> params = command.getParams();

	if (params.empty() || params.size() > 2) {
		//message pas assez d'arguments (ou trop ??)
		return ;
	}
	if (!checkChannelExistence(params[0], channels)) {
		client.enqueueOutput(":myserver 403 " + client.getNickname() + " " + params[0] + " :No such channel");
		return ;
	}
	size_t pos = params[0].find_first_of(',');
	if (pos == std::string::npos)
		handleSingleClientKicking(command, client, clients, channels);
	else
		handleMultipleClientsKicking(params, client, clients, channels);
}

void CommandsProcessingStore::commandInvite(Command& command, Client& requester, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::vector<std::string> params = command.getParams();

	if (params.size() != 2) {
		requester.enqueueOutput(":myserver 461 " + requester.getNickname() + " INVITE :Not enough parameters");
		return ;
	}
	std::string targetNick = strToLowerRFC(params[0]);
	std::string chanName   = strToLowerRFC(params[1]);

	if (!checkChannelExistence(chanName, channels)) {
		requester.enqueueOutput(":myserver 403 " + requester.getNickname() + " " + params[1] + " :No such channel");
		return ;
	}
	Channel* chan = channels[chanName];
	Client* guest = getClientByName(targetNick, clients);
	if (guest == NULL) {
		requester.enqueueOutput(":myserver 401 " + requester.getNickname() + " " + params[0] + " :No such nick/channel");
		return ;
	}
	if (!chan->isMember(requester.getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 442 " + requester.getNickname() + " " + params[1] + " :You're not on that channel");
		return ;
	}
	if (chan->isMember(guest->getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 443 " + requester.getNickname() + " " + guest->getNickname() + " " + params[1] + " :is already on channel");
		return;
	}
	if (chan->isInviteOnly() && !chan->isOperator(requester.getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 482 " + requester.getNickname() + " " + params[1] + " :You're not channel operator");
		return ;
	}
	guest->setInvitationTo(chan->getNormalizedChanName());
	requester.enqueueOutput(":myserver 341 " + requester.getNickname() + " " + guest->getNickname() + " " + params[1]);
	guest->enqueueOutput(requester.getPrefix() + " INVITE " + guest->getNickname() + " :" + params[1]);
}

void CommandsProcessingStore::commandTopic(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	(void)clients;
	std::vector<std::string> params = command.getParams();

	if (params.empty()) {
		client.enqueueOutput(":myserver 461 " + client.getNickname() + " TOPIC :Not enough parameters");
		return ;
	}
	std::string chanName = strToLowerRFC(params[0]);
	if (!checkChannelExistence(chanName, channels)) {
		client.enqueueOutput(":myserver 403 " + client.getNickname() + " " + params[0] + " :No such channel");
		return ;
	}
	Channel* chan = channels[chanName];
	if (!chan->isMember(client.getNormalizedRfcNickname())) {
		client.enqueueOutput(":myserver 442 " + client.getNickname() + " " + chanName + " :You're not on that channel");
		return ;
	}
	std::string topicNewName = command.getTrailing();
	if (!topicNewName.empty()) {
		if (chan->isTopicRestrict() && !chan->isOperator(client.getNormalizedRfcNickname())) {
			client.enqueueOutput(":myserver 482 " + client.getNickname() + " " + chanName + " :You're not channel operator");
			return ;
		}
		chan->setTopic(topicNewName, &client);

		std::string msg = ":" + client.getPrefix() + " TOPIC " + chanName + " :" + topicNewName;
		chan->broadcastMsg("", msg);
	} else {
		std::string topic = chan->getChanTopic();
		if (!topic.empty()) {
			client.enqueueOutput(":myserver 332 " + client.getNickname() + " " + chanName + " :" + topic);
		} else {
			client.enqueueOutput(":myserver 331 " + client.getNickname() + " " + chanName + " :No topic is set");
		}
	}
}

std::vector<modeChange> CommandsProcessingStore::getModeFlags(const std::string& modesStr) {

	bool adding = false;
	std::vector<modeChange> modes;

	for (size_t i = 0; i < modesStr.size(); ++i) {

		char c = modesStr[i];
		if (c == '+') {
			adding = true;
			continue;
		}
		if (c == '-') { adding = false;
			continue;
		}

		modeChange mode;
		mode.mode = c;
		mode.adding = adding;
		modes.push_back(mode);
	}
	return (modes);
}

bool CommandsProcessingStore::setNewChanKey(Client& requester, std::string& param, Channel *channel) {

	if (param.empty()) {
		requester.enqueueOutput(":myserver 461 " + requester.getNickname() + " MODE :Not enough parameters");
		return (false);
	}
	channel->setKey(param);
	return (true);
}

bool CommandsProcessingStore::setChanOperator(Client& requester, std::map<int, Client>& clients, modeChange& currentMode, std::string& param, Channel *channel) {

	std::string targetNick = strToLowerRFC(param);

	if (!channel->isMember(targetNick)) {
		requester.enqueueOutput(":myserver 441 " + requester.getNickname() + " " + param + " " + channel->getChanName() + " :They aren't on that channel");
		return (false);
	}
	Client *newModo = getClientByName(targetNick, clients);
	if (currentMode.adding) {
		channel->addOperator(*newModo);
	} else if (!currentMode.adding) {
		channel->removeOperator(*newModo);
	}
	return (true);
}

bool CommandsProcessingStore::setChanNewUserLimit(Client& client, std::string& param, Channel *channel) {

	char *endPtr;
	errno = 0;

	long userLimit = strtol(param.c_str(), &endPtr, 10);
	if (errno == ERANGE || *endPtr != '\0' || userLimit <= 0) {
		client.enqueueOutput(":myserver 461 " + client.getNickname() + " MODE :Invalid user limit");
		return (false);
	}
	channel->setUserLimit(userLimit);
	return (true);
}

void CommandsProcessingStore::applyModeFlags(Client& client, std::map<int, Client>& clients, std::vector<modeChange>& flags, std::vector<std::string>& params, Channel *chan, size_t& paramIndex)
{
	std::string modeStr;
	std::string modeParams;

	for (std::vector<modeChange>::iterator it = flags.begin(); it != flags.end(); ++it) {
		modeChange currentMode = *it;
		bool success = false;

		switch (currentMode.mode) {
			case 'i':
				chan->setInviteOnly(currentMode.adding);
				success = true;
				break;

			case 't':
				chan->setTopicRestrict(currentMode.adding);
				success = true;
				break;

			case 'k':
				if (currentMode.adding) {
					if (paramIndex >= params.size()) {
						client.enqueueOutput(":myserver 461 " + client.getNickname() + " MODE :Not enough parameters");
					} else {
						success = setNewChanKey(client, params[paramIndex], chan);
						if (success)
							modeParams += " " + params[paramIndex];
						++paramIndex;
					}
				} else {
					chan->removeKey();
					success = true;
				}
			break;

			case 'o':
				if (paramIndex >= params.size()) {
					client.enqueueOutput(":myserver 461 " + client.getNickname() + " MODE :Not enough parameters");
				} else {
					success = setChanOperator(client, clients, currentMode, params[paramIndex], chan);
					if (success)
						modeParams += " " + params[paramIndex];
					++paramIndex;
				}
				break;

			case 'l':
				if (currentMode.adding) {
					if (paramIndex >= params.size()) {
						client.enqueueOutput(":myserver 461 " + client.getNickname() + " MODE :Not enough parameters");
					} else {
						success = setChanNewUserLimit(client, params[paramIndex], chan);
						if (success)
							modeParams += " " + params[paramIndex];
						++paramIndex;
					}
				} else {
					chan->removeUserLimit();
					success = true;
				}
				break;
		}
		if (success) {
			modeStr += (currentMode.adding ? "+" : "-");
			modeStr += currentMode.mode;
		}
	}
	if (!modeStr.empty()) {
		chan->broadcastMsg("", client.getPrefix() + " MODE " + chan->getChanName() + " " + modeStr + modeParams);
	}
}

void CommandsProcessingStore::displayChannelParameters(std::string& channelName, Client& requester, std::map<std::string, Channel*> channels) {

	std::string normalizedChanName = strToLowerRFC(channelName);
	if (!checkChannelExistence(normalizedChanName, channels)) {
		requester.enqueueOutput(":myserver 403 " + requester.getNickname() + " " + channelName + " :No such channel");
		return ;
	}
	Channel *chan = channels[normalizedChanName];

	if (!chan->isMember(requester.getNormalizedRfcNickname())) {
		requester.enqueueOutput(":myserver 442 " + requester.getNickname() + " " + channelName + " :You're not on that channel");
		return ;
	}

	std::string modeStr = "+";
	std::string modeParams;

	if (chan->isInviteOnly())
		modeStr += "i";
	if (chan->isTopicRestrict())
		modeStr += "t";
	if (chan->isKeyProtected()) {
		modeStr += "k";
		modeParams += " " + chan->getKey();
	}
	if (chan->hasUserLimit()) {
		modeStr += "l";
		std::ostringstream oss;
		oss << chan->getUserLimit();
		modeParams += " " + oss.str();
	}

	requester.enqueueOutput(":myserver 324 " + requester.getNickname() + " " + channelName + " " + modeStr + modeParams);

	std::ostringstream oss;
	oss << chan->getCreationTime();
	requester.enqueueOutput(":myserver 329 " + requester.getNickname() + " " + channelName + " " + oss.str());
}

void CommandsProcessingStore::commandMode(Command& command, Client& client, std::map<int, Client>& clients, std::map<std::string, Channel*>& channels) {

	std::vector<std::string> params = command.getParams();

	if (params.size() == 1) {
		(displayChannelParameters(params[0], client, channels));
		return ;
	} else {
		std::string chanName = strToLowerRFC(params[0]);
		if (!checkChannelExistence(chanName, channels)) {
			client.enqueueOutput(":myserver 403 " + client.getNickname() + " " + chanName + " :No such channel");
			return ;
		}
		Channel *chan = channels[chanName];
		if (!chan->isMember(client.getNormalizedRfcNickname())) {
			client.enqueueOutput(":myserver 442 " + client.getNickname() + " " + chanName + " :You're not on that channel");
			return ;
		}
		if (!chan->isOperator(client.getNormalizedRfcNickname())) {
			client.enqueueOutput(":myserver 482 " + client.getNickname() + " " + chanName + " :You're not channel operator");
			return ;
		}
		if (params[1].empty() || (params[1][0] != '+' && params[1][0] != '-')) {
			return ;
		}
		size_t paramIndex = 1;
		while (paramIndex < params.size()) {
			std::string arg = params[paramIndex];
			if (arg.empty()) {
				++paramIndex;
				continue;
			}
			if (arg[0] == '+' || arg[0] == '-') {
				std::vector<modeChange> modeFlags = getModeFlags(arg);
				++paramIndex;
				applyModeFlags(client, clients, modeFlags, params, chan, paramIndex);
			} else {
				++paramIndex;
			}
		}
	}
}

CommandsProcessingStore::CommandProcessPtr CommandsProcessingStore::getCommandProcess(Command& command) {

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
		case CMD_KICK:
			return (&CommandsProcessingStore::commandKick);
		case CMD_INVITE:
			return (&CommandsProcessingStore::commandInvite);
		case CMD_TOPIC:
			return (&CommandsProcessingStore::commandTopic);
		case CMD_MODE:
			return (&CommandsProcessingStore::commandMode);
		case CMD_UNKNOWN:
			return (&CommandsProcessingStore::unknownCommand);
	}
}
