#include "Server.hpp"

Server::Server(void) : _serverSocket(-1) {}

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

void	Server::handleNewClient(void) {

	try {
		int	clientFd = acceptClient();
		if (clientFd != -1) {
			//setClientConversation(serverFd, clientFd);
			addClientToInterestList(clientFd);
			std::cout << GREEN << "[client] =" << "= " << clientFd << " ==> is now connected with [SERVER]" RESET << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "acceptNewClient error: " << e.what() << std::endl;
	}
}

int Server::getServerSocket(void) {

	return (_serverSocket);
}
