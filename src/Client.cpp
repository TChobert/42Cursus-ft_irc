#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _isRegistered(false), _isResponsePending(false), _nickname("*") {}

Client::~Client(void) {} // TO COMPLETE

const char * Client::CRLF = "\r\n";

///// GETTERS /////

int Client::getFd(void) const {
	return (_fd);
}

bool Client::getResponsePending(void) const {
	return (_isResponsePending);
}

std::vector<Command>& Client::getCommands(void) {
	return _commands;
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

bool Client::isRegistered(void) const {
	return (_isRegistered);
}

std::string Client::getPrefix(void) const {
	return (":" + _nickname + "!" + _username + "@" + _hostname);
}

std::string& Client::getInputBuffer(void) {
	return (_inputBuffer);
}

std::string& Client::getOutputBuffer(void) {
	return (_outputBuffer);
}

std::string Client::getNormalizedRfcNickname(void) const {

	std::string nick = getNickname();

	for (size_t i = 0; i < nick.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(nick[i]);

		if (c >= 'A' && c <= 'Z')
			nick[i] = static_cast<char>(c + 32);
		else if (c == '[')
			nick[i] = '{';
		else if (c == ']')
			nick[i] = '}';
		else if (c == '\\')
			nick[i] = '|';
		else if (c == '^')
			nick[i] = '~';
	}
	return (nick);
}

bool Client::isCrlfInInput(void) const {
	return (_inputBuffer.find_first_of(CRLF) != std::string::npos);
}

const std::string& Client::getQuitMessage(void) const {
	return (_quitMessage);
}

bool Client::getDisconnectionStatus(void) const {
	return (_isDisconnectionPending);
}

///// SETTERS /////

void Client::setResponsePending(bool mode) {
	_isResponsePending = mode;
}

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

void Client::setRegistered(bool reg) {
	_isRegistered =  reg;
}


void Client::setDisconnectionStatus(bool status) {
	_isDisconnectionPending = status;
}

void Client::appendInput(const char *input, const size_t len) {
	_inputBuffer.append(input, len);
}

void Client::enqueueOutput(const std::string& output) {
	_outputBuffer += output + CRLF;
	setResponsePending(true);
}

void Client::flushInputBuffer(void) {
	_inputBuffer.clear();
}

void Client::flushOutputBuffer(void) {
	_outputBuffer.clear();
}

void Client::addCommand(Command& command) {
	_commands.push_back(command);
}

void Client::setQuitMessage(std::string& quitMessage) {
	_quitMessage = quitMessage;
}
