#ifndef CQ_EVENT_H
#define CQ_EVENT_H

#include <regex>
#include "cq_msg.h"

typedef enum eventTriggerType { TRIG_NEVER = 0, TRIG_ALWAYS = 1,\
								MSG_MATCH = 8, MSG_CONTAIN = 9, MSG_REGEX = 10, TRIG_CUSTOMIZE = 11,\
								TIME_MATCH = 16, TIME_PERIOD = 17, TIME_CUSTOMIZE = 18};
typedef enum eventType { EVENT_NONE = 0, EVENT_ALL = 1, EVENT_PRIVATE = 2, EVENT_GROUP = 4};
typedef enum messageCodeType { CODE_UNICODE = 0, CODE_UTF8 = 1 };

typedef struct eventTag {
	//event index
	int index;
	//permission level needed to trigger this event.
	int permission;
	//name of event
	std::string name;
	//usage example of event
	std::string example;
	//description of event
	std::string description;
};

class CQEvent {
public:
	//descriptions of event, optional.
	eventTag tag;

	//using eventType
	int event_type;

	CQEvent(int evt_type = eventTriggerType::TRIG_NEVER);

	virtual ~CQEvent();

	//Auto check, ban & unban according to event_type.
	int isBanned(std::string id);
	int ban(std::string id);
	int unban(std::string id);

	//Group
	int isBannedGroup(std::string id);
	int banGroup(std::string id);
	int unbanGroup(std::string id);

	//User
	int isBannedUser(std::string id);
	int banUser(std::string id);
	int unbanUser(std::string id);

	const std::vector<std::string>& getBannedGroups() const;
	const std::vector<std::string>& getBannedUsers() const;

protected:
	std::vector<std::string> banned_group;
	std::vector<std::string> banned_user;

};

template<typename _T>
int searchFromVector(const std::vector<_T>& vec, _T key) {
	for (auto iter = vec.begin(); iter != vec.end(); iter++) {
		if (*iter == key) {
			return 1;
		}
	}
	return 0;
}

template <typename _T>
size_t searchIndexFromVector(const std::vector<_T>& vec, _T key) {
	size_t sz = vec.size();
	for (size_t i = 0; i < sz; i++) {
		if (vec[i] == key) {
			return i;
		}
	}
	return -1;
}

#endif // !CQ_EVENT