#include "Command.hpp"

Command::Command(void) {}

Command::Command(const std::string& command) : _command(command) {}

Command::~Command(void) {}

// GETTERS
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

// SETTERS
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
