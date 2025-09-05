#pragma once

#include <string>
#include <vector>
#include <unistd.h>
#include <algorithm>

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
	std::vector<std::string> _invitedTo;

	bool _isRegistered;
	bool _isResponsePending;
	bool _needEpollReset;
	bool _disconnectionPending;

	std::string _nickname;
	std::string _username;
	std::string _realname;
	std::string _hostname;
	std::string _quitMessage;

	public:

	authProcessFlags authProcessStatus;

	Client(int fd);
	~Client(void);

	int getFd(void) const;
	bool getResponsePending(void) const;
	const std::string& getNickname(void) const;
	const std::string& getUsername(void) const;
	const std::string& getRealname(void) const;
	const std::string& getHostname(void) const;
	std::string getNormalizedRfcNickname(void) const;
	std::vector<Command>& getCommands(void);
	std::string& getInputBuffer(void);
	std::string& getOutputBuffer(void);
	std::string getPrefix(void) const;
	std::string& getQuitMessage(void);
	bool isRegistered(void) const;
	bool isCrlfInInput(void) const;
	bool getDisconnectionStatus(void) const;
	bool getEpollReset(void) const;
	bool isInvitedTo(const std::string& chanName) const;

	void setResponsePending(bool mode);
	void setNickname(const std::string& nick);
	void setUsername(const std::string& username);
	void setRealname(const std::string& realname);
	void setHostname(const std::string& hostname);
	void setRegistered(bool reg);
	void appendInput(const char *input, const size_t len);
	void enqueueOutput(const std::string& output);
	void flushInputBuffer(void);
	void flushOutputBuffer(void);
	void addCommand(Command& command);
	void setDisconnectionStatus(void);
	void setQuitMessage(std::string& message);
	void setEpollReset(bool status);
	void setInvitationTo(std::string chanName);
};
