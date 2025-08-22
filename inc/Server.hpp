#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdexcept>
#include <map>
#include <unistd.h>
#include <sstream>
#include <netinet/in.h>

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define MAX_EVENTS 64

class Client;

class Server {

	private:

	uint16_t _port;
	int _serverSocket;
	int _epollFd;
	std::map<int, Client> _clients;

	public:

	Server(void);
	~Server(void);
	int getServerSocket(void);
	void setSocketNonBlocking(int fd);
	int acceptClient(void);
	void addClientToInterestList(int clientFd);
	void handleNewClient(void);
};
