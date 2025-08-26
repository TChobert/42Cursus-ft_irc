#include "CommandsProcessingStore.hpp"

CommandsProcessingStore::CommandsProcessingStore(void) {}

CommandsProcessingStore::~CommandsProcessingStore(void) {}

CommandsProcessingStore::CommandProcessPtr CommandsProcessingStore::getCommandProcess(Command& command) {

	switch(command.getCommandType()) {
		case CMD_PASS:
			return (&CommandPass);
		case CMD_NICK:
			return (&CommandNick);
		case CMD_USER:
			return (&CommandUser);
	}
}
