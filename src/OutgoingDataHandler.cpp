#include "OutgoingDataHandler.hpp"

OutgoingDataHandler::OutgoingDataHandler(void) {}

OutgoingDataHandler::~OutgoingDataHandler(void) {}

sendingStatus OutgoingDataHandler::handle(Client& client) {

	std::cout << "SENDING FUNCTION !!!" << std::endl;

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
			return (EVERYTHING_SENT);
		}
		else {
			client.setResponsePending(true);
			return (REMAINING_DATA);
		}
	} else if (sent == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return (REMAINING_DATA);
		} else {
			perror("send");
			client.setResponsePending(false);
			return (ERROR);
		}
	}
	return (ERROR);
}
