#pragma once

#include <iostream>
#include <string>
#include <exception>

struct serverLaunchInfo {
	uint16_t port;
	std::string pswd;
};

class ProgramInputParser {

	private:

	void getPort(std::string port, serverLaunchInfo& info);
	void getPswd(std::string pswd, serverLaunchInfo& info);

	public:

	ProgramInputParser(void);
	~ProgramInputParser(void);

	serverLaunchInfo parse(std::string port, std::string pswd);

	class InvalidPortException : public std::exception {
		virtual const char * what() const throw();
	};

	class InvalidPswdException : public std::exception {
		virtual const char * what() const throw();
	};
};
