#include "msg_event_register.h"

msgEventRegister::msgEventRegister() {
}

msgEventRegister::~msgEventRegister() {
	this->m_events.clear();
	this->m_regfuncs.clear();
}

void msgEventRegister::append(void(*reg_func)(msgEventList&)) {
	this->m_regfuncs.push_back(reg_func);
}

int msgEventRegister::registerEvents() {
	int cnt = 0;
	for (auto iter = this->m_regfuncs.begin(); iter != this->m_regfuncs.end(); iter++) {
		(*iter)(this->m_events);
		cnt++;
	}
	return cnt;
}

int msgEventRegister::executeEvents(CQmsg& msg) {
	int cnt = 0;
	for (auto iter = this->m_events.begin(); iter != this->m_events.end(); iter++) {
		if ((iter->trig(msg)) > 0) {
			cnt++;
			PRINTLOG("Event %s has been triggered.\n", iter->tag.name.c_str());
		}
	}
	return cnt;
}

size_t msgEventRegister::eventSize() {
	return m_events.size();
}

int msgEventRegister::generateHelp(std::string& help_str, int max_permission, int event_type) {
	size_t cnt = 0;
	size_t index_tmp = 0, sz = this->m_events.size();
	std::vector<int> index_permissions;
	std::vector<size_t> index_output;
	std::vector<std::vector<int>> index_events;
	help_str.clear();
	for (size_t i = 0; i < sz; i++) {
		if (event_type == EVENT_ALL || event_type == this->m_events[i].event_type || this->m_events[i].event_type == EVENT_ALL) {
			if ((this->m_events[i].tag.permission <= max_permission) && (!this->m_events[i].tag.description.empty())) {
				index_tmp = searchIndexFromVector<int>(index_permissions, this->m_events[i].tag.permission);
				if (index_tmp != -1) {
					index_events[index_tmp].emplace_back(i);
				}
				else {
					index_permissions.emplace_back(this->m_events[i].tag.permission);
					index_events.emplace_back();
					index_events.back().emplace_back(i);
				}
			}
		}
	}
	sz = index_permissions.size();
	for (size_t i = 0; i < sz; i++) {
		index_output.emplace_back(i);
	}
	//sort
	for (size_t i = 0; i < sz - 1; i++) {
		cnt = i;
		for (size_t j = i + 1; j < sz; j++) {
			if (index_permissions[i] > index_permissions[j]) {
				std::swap<int>(index_permissions[i], index_permissions[j]);
				std::swap<size_t>(index_output[i], index_output[j]);
			}
			else {
				cnt++;
			}
		}
		if (cnt == sz - 1) {
			break;
		}
	}
	//output
	std::string permission_str;
	for (size_t i = 0; i < sz; i++) {
		permission_str = std::to_string(index_permissions[i]);
		for (auto iter = index_events[index_output[i]].begin(); iter != index_events[index_output[i]].end(); iter++) {
			CQMsgEvent& evt_tmp = this->m_events[*iter];
			help_str.append("[").append(permission_str).append("] ").append(evt_tmp.tag.name);
			help_str.append("(");
			switch (evt_tmp.event_type) {
			case eventType::EVENT_ALL:
				help_str.append("Ⱥ/˽");
				break;
			case eventType::EVENT_GROUP:
				help_str.append("Ⱥ");
				break;
			case eventType::EVENT_PRIVATE:
				help_str.append("˽");
				break;
			}
			help_str.append("): ").append(evt_tmp.tag.example).append("\n");
		}
	}
}

msgEventList& msgEventRegister::getEvents() {
	return this->m_events;
}
