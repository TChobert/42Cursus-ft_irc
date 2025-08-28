#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <stdint.h>
#include <stdlib.h>

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

	void parse(std::string port, std::string pswd, serverLaunchInfo& info);

	class InvalidPortException : public std::exception {
		virtual const char * what() const throw();
	};

	class InvalidPswdException : public std::exception {
		virtual const char * what() const throw();
	};
};
