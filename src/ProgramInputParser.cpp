#include "ProgramInputParser.hpp"

ProgramInputParser::ProgramInputParser(void) {}

ProgramInputParser::~ProgramInputParser(void) {}

void ProgramInputParser::getPswd(std::string pswd, serverLaunchInfo& info) {

	if (pswd.empty()) {
		//throw InvalidPswdException;
	}
	size_t size = pswd.size();
	if (size > 10) {
		//throw InvalidPswdException;
	}
	for (size_t i = 0; i < size; ++i) {
		if (!std::isalnum(pswd[i])) {
			//throw InvalidPswdException;
		}
	}
	info.pswd = pswd;
}

void ProgramInputParser::getPort(std::string port, serverLaunchInfo& info) {

	char * endPtr;
	long portValue = std::strtol(port.c_str(), &endPtr, 10);
	if (*endPtr != '\0') {
		//throw InvalidPortException;
	} else if (portValue < 0 || portValue > 65535) {
		//throw InvalidPortException;
	}
	info.port = static_cast<uint16_t>(portValue);
}

serverLaunchInfo ProgramInputParser::parse(std::string port, std::string pswd) {

	serverLaunchInfo info;

	getPort(port, info);
	getPswd(port, info);
	return (info);
}

// EXCEPTIONS //

const char *ProgramInputParser::InvalidPortException::what() const throw() {
	return "Invalid port. Must be a valid number between 0 and 65535";
}

const char *ProgramInputParser::InvalidPswdException::what() const throw() {
	return "Invalid server password. Must be 10 characters (digits or lettres) long max";
}
