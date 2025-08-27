#pragma once

#include <string>
#include <vector>
#include <unistd.h>

#include "authFlags.hpp"
#include "Command.hpp"

enum ExecutionStatus {
	NO_EXECUTION_NEEDED,
	READY_TO_EXECUTE,
	DISCONNECTION
};

class Client {

	private:

	static const char * CRLF;
	int _fd;
	std::string _inputBuffer;
	std::string _outputBuffer;
	std::vector<Command> _commands;

	bool _isAuthentificated;
	bool _isRegistered;
	bool _isResponsePending;

	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _hostname;

	public:

	authProcessFlags authProcessStatus;

	Client(int fd);
	~Client(void);

	int getFd(void);
	bool getResponsePending(void) const;
	const std::string& getNickname(void) const;
	const std::string& getUsername(void) const;
	const std::string& getRealname(void) const;
	const std::string& getHostname(void) const;
	std::string getLowerNickname(void) const;
	std::vector<Command>& getCommands(void);
	std::string& getInputBuffer(void);
	std::string& getOutputBuffer(void);
	std::string getPrefix(void) const;
	bool isAuthentificated(void) const;
	bool isRegistered(void) const;
	bool isCrlfInInput(void) const;

	void setResponsePending(bool mode);
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
	void addCommand(Command& command);

	// void joinChannel(const std::string& channel);
	// void leaveChannel(const std::string& channel);
	// bool isInChannel(const std::string& channel) const;
};
