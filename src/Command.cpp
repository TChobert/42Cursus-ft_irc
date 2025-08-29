#include "Command.hpp"

std::map<std::string, commandType> Command::_typesDictionary;

Command::Command(void) {}

Command::Command(commandType type, const std::string& command) : _type(type), _command(command) {
	initTypesDicitonary();
}

Command::~Command(void) {}

// GETTERS

const commandType& Command::getCommandType(void) const {
	return (_type);
}

const std::string& Command::getPrefix(void) const {
	return (_prefix);
}

const std::string& Command::getCommand(void) const {
	return (_command);
}

const std::string& Command::getParam(size_t position) const {
	if (position >= _params.size()) {
		static const std::string empty = "";
		return (empty);
	}
	return _params[position];
}

const std::vector<std::string>& Command::getParams(void) const {
	return (_params);
}

const std::string& Command::getTrailing(void) const {
	return (_trailing);
}

void Command::printCommand(void) const {

	std::cout << "==> COMMAND ==>" << std::endl;
	std::cout << "Command PREFIX = " << getPrefix() << std::endl;
	std::cout << "Command type = " << getCommand();
	std::cout << "Command PARAMS = " << std::endl;
	for (size_t i = 0; i < _params.size(); ++ i) {
		std::cout << _params[i] << std::endl;
	}
	std::cout << "COMMAND TRAILING = " << getTrailing() << std::endl;
}

// SETTERS

void Command::initTypesDicitonary(void) {
	_typesDictionary["PASS"] = CMD_PASS;
	_typesDictionary["NICK"] = CMD_NICK;
	_typesDictionary["USER"] = CMD_USER;
	//_typesDictionary["JOIN"] = CMD_JOIN;
	_typesDictionary["PRIVMSG"] = CMD_PRIVMSG;
	//_typesDictionary["KICK"] = CMD_KICK;
	//_typesDictionary["INVITE"] = CMD_INVITE;
//	_typesDictionary["TOPIC"] = CMD_TOPIC;
	//_typesDictionary["MODE"] = CMD_MODE;
	//_typesDictionary["QUIT"] = CMD_QUIT;
}

void Command::setCommandType(commandType type) {
	_type = type;
}

void Command::setPrefix(const std::string& prefix) {
	_prefix = prefix;
}

void Command::setCommand(const std::string& cmd) {
	_command = cmd;
}

void Command::addParam(const std::string& param) {
	_params.push_back(param);
}

void Command::setParams(const std::vector<std::string>& params) {
	_params = params;
}

void Command::setTrailing(const std::string& trailing) {
	_trailing = trailing;
}
