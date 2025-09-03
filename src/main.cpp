#include <iostream>

#include "ProgramInputParser.hpp"
#include "Server.hpp"

volatile sig_atomic_t gSignalStatus = 0;

int main(int ac, char **av) {

	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);
	if (ac != 3) {
		std::cerr << "Invalid mumber of arguments. A port and a password are required to launch this IRC server" << std::endl;
	}

	ProgramInputParser inputParser;
	serverLaunchInfo launchInfo;
	try {
		inputParser.parse(av[1], av[2], launchInfo);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	Server server(launchInfo.port, launchInfo.pswd);
	try {
		server.initServer();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	try {
		server.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
