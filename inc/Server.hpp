#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdexcept>
#include <map>
#include <unistd.h>
#include <sstream>
#include <netinet/in.h>
#include <cerrno>

#include "Client.hpp"

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define MAX_EVENTS 64

class Server {

	private:

	uint16_t _port;
	int _serverSocket;
	const std::string _pswd;
	int _epollFd;
	struct epoll_event _events[MAX_EVENTS];
	std::map<int, Client> _clients;

	public:

	Server(const uint16_t port, const std::string pswd);
	~Server(void);

	void run(void);
	void handleNotifiedEvents(int fdsNumber);
	void initServer(void);
	void socketInitProcess(void);
	void setSocketImmediatReuse(void);
	void setSocketNonBlocking(void);
	void bindSocket(void);
	void setSocketListeningMode(void);
	void addSocketToEpoll(void);
	void handleNewClient(void);
	int acceptClient(void);
	void setSocketNonBlocking(int fd);
	void addClientToInterestList(int clientFd);
	void setClientID(const int clientFd);
	//void deleteClient(int clientFd);
	//void deleteAllNetwork(void);

	int getServerSocket(void) const;
};
