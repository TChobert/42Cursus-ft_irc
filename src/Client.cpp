#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {}

Client::~Client(void) {} // TO COMPLETE

///// SETTERS /////

const std::string& Client::getNickname(void) const {
	return (_nickname);
}

const std::string& Client::getUsername(void) const {
	return (_username);
}

const std::string& Client::getRealname(void) const {
	return (_realname);
}

const std::string& Client::getHostname(void) const {
	return (_hostname);
}

bool Client::isAuthentificated(void) const {
	return (_isAuthentificated);
}

bool Client::isRegistered(void) const {
	return (_isRegistered);
}

std::string Client::getPrefix(void) const {
	return (":" + _nickname + "!" + _username + "@" + _hostname);
}

///// SETTERS /////

void Client::setNickname(const std::string& nick) {
	_nickname = nick;
}

void Client::setUsername(const std::string& username) {
	_username = username;
}

void Client::setRealname(const std::string& realname) {
	_realname = realname;
}

void Client::setHostname(const std::string& hostname) {
	_hostname = hostname;
}

void Client::setAuthStatus(bool auth) {
	_isAuthentificated = auth;
}

void Client::setRegistered(bool reg) {
	_isRegistered =  reg;
}
