#include "Command.hpp"

Command::Command(void) {}

Command::Command(commandType type, const std::string& command) : _type(type), _command(command) {}

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

	std::cout << GREEN;
	std::cout << "==> COMMAND ==>" << std::endl;
	std::cout << "Command type: " << getCommand() << std::endl;
	std::cout << "Command parameters: " << std::endl;
	for (size_t i = 0; i < _params.size(); ++ i) {
		std::cout << _params[i] << std::endl;
	}
	std::string trailing = getTrailing();
	if (trailing.empty()) {
		std::cout << "Command trailing: none" << std::endl;
	}
	std::cout << "Command trailing: " << getTrailing() << std::endl;
	std::cout << RESET;
}

// SETTERS

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
