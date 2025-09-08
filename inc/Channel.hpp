#pragma once

#include <map>
#include <string>
#include <set>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include "Client.hpp"

class Channel {

	private:

	std::string _name;
	std::string _topic;
	std::string _key;
	std::string _topicSetter;
	bool _inviteOnly;
	bool _topicRestrict;
	long _userLimit;
	std::time_t _creationTime;
	std::time_t _topicTimestamp;

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
	long getUserLimit(void) const;
	const std::string& getChanName(void) const;
	const std::string& getChanTopic(void) const;
	std::string getMembersListForIRC(void) const;
	const std::string& getKey(void) const;
	bool hasUserLimit(void) const;
	time_t getCreationTime(void) const;
	long getMembersNumber(void) const;
	std::set<std::string> getOperators() const;

	void addMember(Client* newMember);
	void removeMember(Client& client);
	void setChanName(const std::string& name);
	void setTopic(const std::string& topic, const Client *setter);
	void broadcastMsg(const std::string& sender, const std::string& message);
	void broadcastQuit(const std::string& message);
	std::string getNormalizedChanName(void);
	void addOperator(Client& newOp);
	void removeOperator(Client& toRemove);
	void setKey(const std::string& key);
	bool checkKey(const std::string& key) const;
	void setInviteOnly(bool mode);
	void setTopicRestrict(bool mode);
	void setUserLimit(long userLimit);
	void removeKey(void);
	void removeUserLimit(void);
};
