#include "OutgoingDataHandler.hpp"

OutgoingDataHandler::OutgoingDataHandler(void) {}

OutgoingDataHandler::~OutgoingDataHandler(void) {}

void OutgoingDataHandler::updateClientEpoll(Client& client, bool needsOutput, int& epollFd) {
    struct epoll_event ev;
    ev.data.fd = client.getFd();
    ev.events = EPOLLIN;
    
    if (needsOutput) {
        ev.events |= EPOLLOUT;
    }
    
    epoll_ctl(epollFd, EPOLL_CTL_MOD, client.getFd(), &ev);
}

sendingStatus OutgoingDataHandler::handle(Client& client, int& epollFd) {

	std::string &dataToSend = client.getOutputBuffer();
    if (dataToSend.empty()) {
        client.setResponsePending(false);
        return (EVERYTHING_SENT);
    }
    
    ssize_t sent = send(client.getFd(), dataToSend.c_str(), dataToSend.size(), 0);
    
    if (sent > 0) {
        dataToSend.erase(0, sent);
        if (dataToSend.empty()) {
            client.setResponsePending(false);
            // ✅ Retire EPOLLOUT car tout envoyé
            updateClientEpoll(client, false, epollFd);
            return (EVERYTHING_SENT);
        } else {
            client.setResponsePending(true);
            // ✅ Active EPOLLOUT car il reste des données
            updateClientEpoll(client, true, epollFd);
            return (REMAINING_DATA);
        }
    } else if (sent == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // ✅ Active EPOLLOUT car on doit réessayer plus tard
            updateClientEpoll(client, true, epollFd);
            return (REMAINING_DATA);
        } else {
            perror("send");
            client.setResponsePending(false);
            return (ERROR);
        }
    }
    return (ERROR);
}
