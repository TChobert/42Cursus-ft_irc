#include "Server.hpp"

Server::Server(const uint16_t port, const std::string pswd) : _port(port), _serverSocket(-1), _pswd(pswd), _executor(pswd) {}

Server::~Server(void) {}

void	Server::setSocketNonBlocking(int fd) {

	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		close(fd);
		std::ostringstream	oss;
		oss << "fcntl failed on client fd: " << fd << " while setting it nonblocking, closing it.";
		throw std::runtime_error(oss.str());
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		close(fd);
		std::ostringstream	oss;
		oss << "fcntl failed on client: " << fd << " while setting it nonblocking, closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	Server::addClientToInterestList(int clientFd) {

	struct	epoll_event	ev;
	ev.data.fd = clientFd;
	ev.events = EPOLLIN;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0) {
		close(clientFd);
		std::ostringstream	oss;
		oss << "Failed to add client socket " << clientFd << " to epoll; closing it.";
		throw std::runtime_error(oss.str());
	}
}

int	Server::acceptClient(void) {

	sockaddr_in	clientAddress;
	std::memset(&clientAddress, 0, sizeof(clientAddress));
	socklen_t	clientLen = sizeof(clientAddress);

	int	clientFd = accept(_serverSocket, (sockaddr *)&clientAddress, &clientLen);
	if (clientFd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return (-1);
		}
		std::ostringstream	oss;
		oss << "Failed to add client to server." << std::endl;
		throw std::runtime_error(oss.str());
	}
	setSocketNonBlocking(clientFd);
	return (clientFd);
}

void Server::setClientID(const int clientFd) {

	Client client(clientFd);
	_clients.insert(std::make_pair(clientFd, client));
}

void	Server::handleNewClient(void) {

	try {
		int	clientFd = acceptClient();
		if (clientFd != -1) {
			setClientID(clientFd);
			addClientToInterestList(clientFd);
			std::cout << GREEN << "[client] =" << "= " << clientFd << " ==> is now connected with [SERVER]" RESET << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "acceptNewClient error: " << e.what() << std::endl;
	}
}

void Server::setSocketImmediatReuse(void) {

	int	opt = 1;
	if (setsockopt(_serverSocket,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::ostringstream	oss;
		oss << "Failed to initialize socket in immediate reuse mode.";
		throw std::runtime_error(oss.str());
	}
}

void Server::setSocketNonBlocking(void) {

	int	flags = fcntl(_serverSocket, F_GETFL, 0);
	if (flags < 0) {
		std::ostringstream	oss;
		oss << "fcntl failed on server socket: " << _serverSocket << " while setting it nonblocking";
		throw std::runtime_error(oss.str());
	}
	if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::ostringstream	oss;
		oss << "fcntl failed on client: " << _serverSocket << " while setting it nonblocking";
		throw std::runtime_error(oss.str());
	}
}

void Server::bindSocket(void) {

	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::ostringstream portStr;
	portStr << _port;

	int ret = getaddrinfo(NULL, portStr.str().c_str(), &hints, &res);
	if (ret != 0 || !res) {
		throw std::runtime_error("getaddrinfo() failed for port " + portStr.str() + ": " + gai_strerror(ret));
	}
	if (bind(_serverSocket, res->ai_addr, res->ai_addrlen) < 0) {
		freeaddrinfo(res);
		throw std::runtime_error("bind() failed on port " + portStr.str() + ": " + strerror(errno));
	}
	freeaddrinfo(res);
}

void Server::setSocketListeningMode(void) {

	if (listen(_serverSocket, SOMAXCONN) < 0) {
		std::ostringstream	oss;
		oss << "Failed to put IRC server socket on listening mode. Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void Server::addSocketToEpoll(void) {

	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = _serverSocket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverSocket, &ev) < 0) {
		std::ostringstream	oss;
		oss << "Failed to add socket: " << _serverSocket << ". Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void Server::socketInitProcess(void) {

	setSocketImmediatReuse();
	setSocketNonBlocking();
	bindSocket();
	setSocketListeningMode();
	addSocketToEpoll();
}

void Server::initServer(void) {

	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	_epollFd = epoll_create1(0);

	if (_serverSocket >= 0 && _epollFd >= 0) {
		try {
			socketInitProcess();
			std::cout << YELLOW << "[SERVER] :: is ready to communicate!" << RESET << std::endl;
		}
		catch (const std::exception& e) {
			close(_serverSocket);
			std::cerr << RED << "Error while initializing IRC server:" << e.what() << RESET <<std::endl;
		}
	} else {
			std::cerr << RED << "Failed to initialize IRC server socket. EXIT" << RESET << std::endl;
	}
}

///// MAIN PROCESS /////

void Server::handleOutgoingEvent(int fd) {

	sendingStatus status = _outgoingDataHandler.handle(_clients.at(fd));
	if (status == ERROR) {
		//disconnectClient(_clients[fd]);
	}
}

void Server::handleIncomingEvent(int fd) {

	ExecutionStatus status = _incomingDataHandler.handle(_clients.at(fd));

	if (status == READY_TO_EXECUTE) {
		_executor.execute(_clients.at(fd), _clients);
	}
}

void Server::handleNotifiedEvents(int fdsNumber) {

	for (int i = 0; i < fdsNumber; ++i) {

		int currentFd = _events[i].data.fd;
		uint32_t currentEvent = _events[i].events;

		if (currentFd == _serverSocket) {
			handleNewClient();
		}
		else {
			if (currentEvent & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
			// 	handleClientDisconnection(currentFd);
			} else if (currentEvent & EPOLLIN) {
				handleIncomingEvent(currentFd);
			} else if (currentEvent & EPOLLOUT) {
				handleOutgoingEvent(currentFd);
			}
		}
	}
}

void Server::updateEpollInterest(Client& client) {

	struct epoll_event ev;
	ev.data.fd = client.getFd();
	ev.events = EPOLLIN;

	if (client.getResponsePending())
		ev.events |= EPOLLOUT;

	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getFd(), &ev) < 0) {
		std::cerr << "Failed to update epoll interest for client " 
					<< client.getPrefix() << std::endl;
		//disconnectClient(client);
	}
}

void Server::manageEpollInterests(void) {

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			updateEpollInterest(it->second);
}

void Server::run(void) {

	while (true) {

		manageEpollInterests();
		int fdsNumber = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		if (fdsNumber == -1) {
			if (errno == EINTR) {
				continue ;
			}
			else {
				//deleteAllNetwork();
				throw std::runtime_error("Critical epoll_wait error. Server interruption.");
			}
		}
		handleNotifiedEvents(fdsNumber);
	}
}

///// GETTERS /////

int Server::getServerSocket(void) const {

	return (_serverSocket);
}
