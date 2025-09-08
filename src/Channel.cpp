#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name), _topic(""), _key(""), _topicSetter(""), _inviteOnly(false), _topicRestrict(false), _userLimit(0), _creationTime(std::time(NULL)), _topicTimestamp(0) {}

Channel::~Channel(void) {}

// GETTERS //

bool Channel::isInviteOnly(void) const {
	return (_inviteOnly);
}

bool Channel::isKeyProtected(void) const {
	return (!_key.empty());
}

bool Channel::isTopicRestrict(void) const {
	return (_topicRestrict);
}

bool Channel::isEmpty(void) const {
	return (_members.empty());
}

const std::string& Channel::getChanName(void) const {
	return (_name);
}

const std::string& Channel::getKey(void) const {
	return (_key);
}

long Channel::getUserLimit(void) const {
	return (_userLimit);
}

const std::string& Channel::getChanTopic(void) const {
	return (_topic);
}

bool Channel::isMember(const std::string& nickname) const {
	return (_members.count(nickname) != 0);
}

bool Channel::isOperator(const std::string& nickname) const {
	return (_operators.count(nickname) != 0);
}

std::string Channel::getMembersListForIRC(void) const {

	std::string list;
	for (std::map<std::string, Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (!list.empty()) {
			list.append(" ");
		}
		if (_operators.count(it->first))
			list.append("@");
		list.append(it->second->getNickname());
	}
	return (list);
}

bool Channel::checkKey(const std::string& key) const {
	return (_key == key);
}

bool Channel::hasUserLimit(void) const {
	return (_userLimit != 0);
}

time_t Channel::getCreationTime(void) const {
	return (_creationTime);
}

long Channel::getMembersNumber(void) const {
	return (static_cast<long>(_members.size()));
}

// SETTERS //

void Channel::setChanName(const std::string& name) {
	_name = name;
}

void Channel::setTopic(const std::string& topic, const Client *setter) {

	_topic = topic;
	if (setter) {
		_topicSetter = setter->getNormalizedRfcNickname();
	} else {
		_topicSetter = "myserver";
	}
	_topicTimestamp = std::time(NULL);
}

void Channel::setKey(const std::string& key) {
	_key = key;
}

void Channel::addMember(Client* newMember) {

	_members.insert(std::make_pair(newMember->getNormalizedRfcNickname(), newMember));

	std::string joinMsg = ":" + newMember->getPrefix() + " JOIN " + _name;
	broadcastMsg("", joinMsg);

	std::ostringstream oss;
	oss << _topicTimestamp;
	std::string memberList = getMembersListForIRC();
	newMember->enqueueOutput(":myserver 353 " + newMember->getNickname() + " = " + _name + " :" + memberList);
	newMember->enqueueOutput(":myserver 366 " + newMember->getNickname() + " " + _name + " :End of NAMES list");
	if (!_topic.empty()) {
		newMember->enqueueOutput(":myserver 332 " + newMember->getNickname() + " " + _name + " :" + _topic);
		newMember->enqueueOutput(":myserver 333 " + newMember->getNickname() + " " + _name + " " + _topicSetter + " " + oss.str());
	} else {
		newMember->enqueueOutput(":myserver 331 " + newMember->getNickname() + " " + _name + " :No topic is set");
	}
}

void Channel::addOperator(Client& newOp) {
	_operators.insert(newOp.getNormalizedRfcNickname());
}

void Channel::removeOperator(Client& toRemove) {

	std::string nick = toRemove.getNormalizedRfcNickname();
	_operators.erase(nick);
}

void Channel::removeMember(Client& client) {

	const std::string nickname = client.getNormalizedRfcNickname();

	_members.erase(nickname);
	if (_operators.count(nickname))
		_operators.erase(nickname);
}

void Channel::broadcastQuit(const std::string& message) {
	std::vector<Client*> members;
	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		members.push_back(it->second);
	}
	for (size_t i = 0; i < members.size(); ++i) {
		members[i]->enqueueOutput(message);
	}
}

void Channel::broadcastMsg(const std::string& sender, const std::string& message) {
	std::vector<Client*> members;
	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (sender != it->second->getNormalizedRfcNickname()) {
			members.push_back(it->second);
		}
	}
	for (size_t i = 0; i < members.size(); ++i) {
		members[i]->enqueueOutput(message);
	}
}


std::string Channel::getNormalizedChanName(void) {

	std::string name = getChanName();

	for (size_t i = 0; i < name.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(name[i]);

		if (c >= 'A' && c <= 'Z')
			name[i] = static_cast<char>(c + 32);
		else if (c == '[')
			name[i] = '{';
		else if (c == ']')
			name[i] = '}';
		else if (c == '\\')
			name[i] = '|';
		else if (c == '^')
			name[i] = '~';
	}
	return (name);
}

void Channel::setInviteOnly(bool mode) {
	_inviteOnly = mode;
}

void Channel::setTopicRestrict(bool mode) {
	_topicRestrict = mode;
}

void Channel::removeKey(void) {
	_key = std::string();
}

void Channel::removeUserLimit(void) {
	_userLimit = 0;
}

void Channel::setUserLimit(long userLimit) {
	_userLimit = userLimit;
}
