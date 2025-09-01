#pragma once

#include <map>
#include <string>
#include <set>
#include <cstdlib>

#include "Client.hpp"

class Channel {

	private:

	std::string _name;
	std::string _topic;
	std::string _key;
	bool _inviteOnly;
	bool _topicRestrict;
	size_t _userLimit;

	std::map<std::string, Client*> _members;
	std::set<std::string> _operators;

	public:

	Channel(std::string name);
	~Channel(void);

	bool isMember(const std::string& nick) const;
	bool isOperator(const std::string& nick) const;
	bool isInviteOnly(void) const;
	bool isKeyProtected(void) const;
	bool isTopicRestrict(void) const;
	bool isEmpty(void) const;
	size_t getUserLimit(void) const;
	const std::string& getChanName(void) const;
	const std::string& getChanTopic(void) const;
	std::string getMembersListForIRC(void) const;
	const std::string& getKey(void) const;

	void addMember(Client* newMember);
	void removeMember(Client& client);
	void setChanName(const std::string& name);
	void setTopic(const std::string& topic);
	void broadcastMsg(const std::string& sender, const std::string& message);
	std::string getNormalizedChanName(void);
	void addOperator(Client& newOp);
	void setKey(const std::string& key);
	bool checkKey(const std::string& key) const;
};
