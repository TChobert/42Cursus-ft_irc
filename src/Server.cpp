#include "Server.hpp"

extern volatile sig_atomic_t gSignalStatus;

Server::Server(const uint16_t port, const std::string pswd) : _port(port), _serverSocket(-1), _pswd(pswd), _executor(pswd) {}

Server::~Server(void) {}

void signalHandler(int signal) {
	gSignalStatus = signal;
}

void Server::disconnectAllClients(void) {

	std::map<int, Client> toDelete;

	toDelete.swap(_clients);
	for (std::map<int, Client>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		disconnectClient(it->second);
	}
}

void Server::deleteAllChannels(void) {

	std::map<std::string, Channel*> toDelete;

	toDelete.swap(_channels);
	for (std::map<std::string, Channel*>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		if (it->second != NULL)
			delete it->second;
	}
}

void Server::deleteAllNetwork(void) {

	disconnectAllClients();
	deleteAllChannels();
	if (_epollFd >= 0)
		close (_epollFd);
}

void Server::disconnectClient(Client &client) {

	int clientFd = client.getFd();

	std::cout << RED << client.getPrefix() << " is now disconnected from [SERVER]" << RESET << std::endl;
	 if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL) < 0) {
		perror("epoll_ctl DEL");
	}
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ) {
		if (it->second->isMember(client.getNormalizedRfcNickname())) {
			//it->second->broadcastQuit(client.getQuitMessage());
			it->second->removeMember(client);
		}
		if (it->second->isEmpty()) {
			delete it->second;
			std::map<std::string, Channel*>::iterator tmp = it;
			++it;
			_channels.erase(tmp);
		} else {
			++it;
		}
	}
	_clients.erase(clientFd);
	close(clientFd);
}

void Server::handleClientDisconnection(int clientFd) {

	disconnectClient(_clients.at(clientFd));
}

void	Server::setClientSocketNonBlocking(int fd) {

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
	setClientSocketNonBlocking(clientFd);
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
			std::cout << YELLOW << "[SERVER] <::> is ready to communicate!" << RESET << std::endl;
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

void Server::handleIncomingEvent(int fd) {

	ExecutionStatus status = _incomingDataHandler.receiveDataFromClient(_clients.at(fd));

	if (status == READY_TO_EXECUTE) {
		_executor.execute(_clients.at(fd), _clients, _channels);
	}
	if (status == DISCONNECTION) {
		handleClientDisconnection(fd);
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
				handleClientDisconnection(currentFd);
			} else if (currentEvent & EPOLLIN) {
				handleIncomingEvent(currentFd);
			}
		}
	}
}

void Server::disconnectClients() {

	std::vector<int> toDisconnect;

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getDisconnectionStatus()) {
			toDisconnect.push_back(it->first);
		}
	}
	for (size_t i = 0; i < toDisconnect.size(); ++i) {
		disconnectClient(_clients.at(toDisconnect[i]));
	}
}

void Server::handleMessagesToSend(void) {

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getResponsePending() && !it->second.getOutputBuffer().empty()) {
			if (_outgoingDataHandler.sendResponseToClient(it->second, _epollFd) == ERROR)
				handleClientDisconnection(it->first);
			}
	}
}

void Server::run(void) {

	while (gSignalStatus == 0) {

		handleMessagesToSend();
		disconnectClients();
		int fdsNumber = epoll_wait(_epollFd, _events, MAX_EVENTS, 50);
		if (fdsNumber == -1) {
			if (errno == EINTR) {
				continue ;
			}
			else {
				deleteAllNetwork();
				throw std::runtime_error("Critical epoll_wait error. Server interruption.");
			}
		}
		handleNotifiedEvents(fdsNumber);
		handleMessagesToSend();
	}
	deleteAllNetwork();
}

///// GETTERS /////

int Server::getServerSocket(void) const {
	return (_serverSocket);
}
