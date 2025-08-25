#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _isAuthentificated(false), _isRegistered(false) {}

Client::~Client(void) {
	close(_fd);
} // TO COMPLETE

const char * Client::CRLF = "\r\n";

///// SETTERS /////

int& Client::getFd(void) {
	return (_fd);
}

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

const std::string& Client::getInputBuffer(void) const {
	return (_inputBuffer);
}

const std::string& Client::getOutputBuffer(void) const {
	return (_outputBuffer);
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

void Client::appendInput(const char *input, const size_t len) {
	_inputBuffer.append(input, len);
}

void Client::enqueueOutput(const std::string& output) {
	_outputBuffer += output + CRLF;
}

void Client::flushInputBuffer(void) {
	_inputBuffer.clear();
}

void Client::flushOutputBuffer(void) {
	_outputBuffer.clear();
}
