#include "cq_time_event.h"

CQTimeEvent::CQTimeEvent(int tag_index,\
						int tag_permission,\
						std::string tag_name,\
						std::string tag_example,\
						std::string tag_description,\
						int(*event_function)(const aTime& time, void* arg),\
						int(*condition_function)(const aTime& time, void* arg),\
						int trig_typ,\
						int event_typ,\
						int exec_typ){
	this->tag.index = tag_index;
	this->tag.permission = tag_permission;
	this->tag.name = tag_name;
	this->tag.example = tag_example;
	this->tag.description = tag_description;
	this->event_func = event_function;
	this->custom_condition = condition_function;
	this->trig_type = trig_typ;
	this->event_type = event_typ;
	this->exec_type = exec_typ;
}

CQTimeEvent::~CQTimeEvent(){

}

int CQTimeEvent::trig(const aTime& time, void* arg){
	if (this->checkTriggerCondition(time, arg)) {
		std::thread event_thread(event_func, std::cref(time), arg);
		switch (this->exec_type) {
		case EXEC_JOIN:
			if (event_thread.joinable()) {
				event_thread.join();
				return 1;
			}
			else {
				return 0;
			}

		case EXEC_DETACH:
			event_thread.detach();
			return 1;

		default:
			return 0;
		}
	}
    return 0;
}

int CQTimeEvent::checkTriggerCondition(const aTime& time, void* arg){
	switch (this->trig_type) {
	case eventTriggerType::TIME_MATCH:
		for (auto iter = this->trig_time.begin(); iter != this->trig_time.end(); iter++) {
			if (setTimeCompare(time, *iter) == 0) {
				return 1;
			}
		}
		return 0;

	case eventTriggerType::TIME_PERIOD:
		for (auto iter = this->trig_period.begin(); iter != this->trig_period.end(); iter++) {
			if (setTimeCompare(time, iter->begin) >= 0 && setTimeCompare(time, iter->end) <= 0) {
				return 1;
			}
		}
		return 0;

	case eventTriggerType::TIME_CUSTOMIZE:
		return this->custom_condition(time, arg);

	case eventTriggerType::TRIG_ALWAYS:
		return 1;

	case eventTriggerType::TRIG_NEVER:
		return 0;

	default:
		return 0;
	}
}

int setTimeCompare(const aTime& current_time, const aTime& set_time) {
	if (set_time.year() >= 0) {
		if (current_time.year() > set_time.year()) {
			return 1;
		}
		else if (current_time.year() < set_time.year()) {
			return -1;
		}
	}
	if (set_time.month() >= 0) {
		if (current_time.month() > set_time.month()) {
			return 1;
		}
		else if (current_time.month() < set_time.month()) {
			return -1;
		}
	}
	if (set_time.mday() >= 0) {
		if (current_time.mday() > set_time.mday()) {
			return 1;
		}
		else if (current_time.mday() < set_time.mday()) {
			return -1;
		}
	}
	if (set_time.wday() >= 0) {
		if (current_time.wday() > set_time.wday()) {
			return 1;
		}
		else if (current_time.wday() < set_time.wday()) {
			return -1;
		}
	}
	if (set_time.hour() >= 0) {
		if (current_time.hour() > set_time.hour()) {
			return 1;
		}
		else if (current_time.hour() < set_time.hour()) {
			return -1;
		}
	}
	if (set_time.mint() >= 0) {
		if (current_time.mint() > set_time.mint()) {
			return 1;
		}
		else if (current_time.mint() < set_time.mint()) {
			return -1;
		}
	}
	if (set_time.sec() >= 0) {
		if (current_time.sec() > set_time.sec()) {
			return 1;
		}
		else if (current_time.sec() < set_time.sec()) {
			return -1;
		}
	}
	return 0;
}