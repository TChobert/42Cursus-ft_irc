#include "Channel.hpp"

Channel::Channel(void) {}

Channel::~Channel(void) {}

// GETTERS //

const std::string& Channel::getChanName(void) const {
	return (_name);
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

// SETTERS //

void Channel::setChanName(const std::string& name) {
	_name = name;
}

void Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void Channel::addMember(Client* newMember) {

	_members.insert(std::make_pair(newMember->getNickname(), newMember));
}

void Channel::removeMember(const std::string& nickname) {

	_members.erase(nickname);
	if (_operators.count(nickname))
		_operators.erase(nickname);
}

void Channel::broadcastMsg(const std::string& sender, const std::string& message) {

	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (sender != it->second->getNormalizedRfcNickname()) {
			it->second->enqueueOutput(message);
		}
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
