#include "cq_event.h"

CQEvent::CQEvent(int evt_type) {
	this->event_type = evt_type;
}

CQEvent::~CQEvent() {

}

int CQEvent::isBanned(std::string id) {
	switch (this->event_type) {
	case eventType::EVENT_ALL:
		if (this->isBannedGroup(id) || this->isBannedUser(id)) {
			return 1;
		}
		else {
			return 0;
		}
	case eventType::EVENT_GROUP:
		return this->isBannedGroup(id);
	case eventType::EVENT_PRIVATE:
		return this->isBannedUser(id);
	}
	return 0;
}

int CQEvent::ban(std::string id) {
	int bg = 0, bu = 0;
	switch (this->event_type) {
	case eventType::EVENT_ALL:
		bg = this->banGroup(id);
		bu = this->banUser(id);
		if (bg == 1 || bu == 1) {
			return 1;
		}
		else {
			return 0;
		}
	case eventType::EVENT_GROUP:
		return this->banGroup(id);
	case eventType::EVENT_PRIVATE:
		return this->banUser(id);
	}
	return 0;
}

int CQEvent::unban(std::string id) {
	int unbg = 0, unbu = 0;
	switch (this->event_type) {
	case eventType::EVENT_ALL:
		unbg = this->unbanGroup(id);
		unbu = this->unbanUser(id);
		if (unbg == 1 || unbu == 1) {
			return 1;
		}
		else {
			return 0;
		}
	case eventType::EVENT_GROUP:
		return this->unbanGroup(id);
	case eventType::EVENT_PRIVATE:
		return this->unbanUser(id);
	}
	return 0;
}

int CQEvent::isBannedGroup(std::string id) {
	if (searchFromVector<std::string>(this->banned_group, id)) {
		return 1;
	}
	else {
		return 0;
	}
}

int CQEvent::banGroup(std::string id) {
	if (!this->isBannedGroup(id)) {
		this->banned_group.push_back(id);
		return 1;
	}
	else {
		return 0;
	}
}

int CQEvent::unbanGroup(std::string id) {
	for (auto iter = this->banned_group.begin(); iter != this->banned_group.end(); iter++) {
		if (*iter == id) {
			this->banned_group.erase(iter);
			return 1;
		}
	}
	return 0;
}

int CQEvent::isBannedUser(std::string id) {
	if (searchFromVector<std::string>(this->banned_user, id)) {
		return 1;
	}
	else {
		return 0;
	}
}

int CQEvent::banUser(std::string id) {
	if (!this->isBannedUser(id)) {
		this->banned_user.push_back(id);
		return 1;
	}
	else {
		return 0;
	}
}

int CQEvent::unbanUser(std::string id) {
	for (auto iter = this->banned_user.begin(); iter != this->banned_user.end(); iter++) {
		if (*iter == id) {
			this->banned_user.erase(iter);
			return 1;
		}
	}
	return 0;
}

const std::vector<std::string>& CQEvent::getBannedGroups() const{
	return this->banned_group;
}

const std::vector<std::string>& CQEvent::getBannedUsers() const{
	return this->banned_user;
}
