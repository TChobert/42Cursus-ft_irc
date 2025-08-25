#pragma once

#include <string>
#include <unistd.h>

#include "authFlags.hpp"

class Client {

	private:

	static const char * CRLF;
	int _fd;
	std::string _inputBuffer;
	std::string _outputBuffer;

	bool _isAuthentificated;
	bool _isRegistered;

	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _hostname;

	public:

	authProcessFlags authProcessStatus;

	Client(int fd);
	~Client(void);

	int& getFd(void);
	const std::string& getNickname(void) const;
	const std::string& getUsername(void) const;
	const std::string& getRealname(void) const;
	const std::string& getHostname(void) const;
	const std::string& getInputBuffer(void) const;
	const std::string& getOutputBuffer(void) const;
	std::string getPrefix(void) const;
	bool isAuthentificated(void) const;
	bool isRegistered(void) const;

	void setNickname(const std::string& nick);
	void setUsername(const std::string& username);
	void setRealname(const std::string& realname);
	void setHostname(const std::string& hostname);
	void setAuthStatus(bool auth);
	void setRegistered(bool reg);
	void appendInput(const char *input, const size_t len);
	void enqueueOutput(const std::string& output);
	void flushInputBuffer(void);
	void flushOutputBuffer(void);

	// void joinChannel(const std::string& channel);
	// void leaveChannel(const std::string& channel);
	// bool isInChannel(const std::string& channel) const;
};
