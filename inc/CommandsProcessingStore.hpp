#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Client.hpp"

class CommandsProcessingStore {

	public:

	typedef void (CommandsProcessingStore::*CommandProcessPtr)(Client&, std::map<int, Client>&);

	CommandsProcessingStore(void);
	~CommandsProcessingStore(void);

	CommandProcessPtr getCommandProcess(Command& command);
};
