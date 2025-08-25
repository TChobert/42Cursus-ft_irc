#pragma once

#include <string>
#include <vector>
#include <stdexcept>

class Command {

	private:
		std::string _prefix;
		std::string _command;
		std::vector<std::string> _params;
		std::string _trailing;

	public:
		Command(void);
		Command(const std::string& command);
		~Command(void);

	// GETTERS

	const std::string& getPrefix(void) const;
	const std::string& getCommand(void) const;
	const std::string& getParam(size_t position) const;
	const std::vector<std::string>& getParams(void) const;
	const std::string& getTrailing(void) const;

	// SETTERS

	void setPrefix(const std::string& prefix);
	void setCommand(const std::string& cmd);
	void addParam(const std::string& param);
	void setParams(const std::vector<std::string>& params);
	void setTrailing(const std::string& trailing);
};
