#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

enum commandType {
	CMD_UNKNOWN = 0,
	CMD_CAP,
	CMD_PING,
	CMD_PASS,
	CMD_NICK,
	CMD_USER,
	CMD_JOIN,
	CMD_PRIVMSG,
	CMD_KICK,
	CMD_INVITE,
	CMD_TOPIC,
	CMD_MODE,
	CMD_QUIT
};

class Command {

	private:

		commandType _type;
		std::string _prefix;
		std::string _command;
		std::vector<std::string> _params;
		std::string _trailing;

	public:

		static std::map<std::string, commandType> _typesDictionary;
		Command(void);
		Command(commandType type, const std::string& command);
		~Command(void);

	// GETTERS

	const commandType& getCommandType(void) const;
	const std::string& getPrefix(void) const;
	const std::string& getCommand(void) const;
	const std::string& getParam(size_t position) const;
	const std::vector<std::string>& getParams(void) const;
	const std::string& getTrailing(void) const;
	void printCommand(void) const;

	// SETTERS

	void initTypesDicitonary(void);
	void setCommandType(commandType type);
	void setPrefix(const std::string& prefix);
	void setCommand(const std::string& cmd);
	void addParam(const std::string& param);
	void setParams(const std::vector<std::string>& params);
	void setTrailing(const std::string& trailing);
};
