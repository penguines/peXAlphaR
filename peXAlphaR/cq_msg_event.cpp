#include "cq_msg_event.h"

CQMsgEvent::CQMsgEvent(int tag_index, \
						int tag_permission, \
						std::string tag_name, \
						std::string tag_example, \
						std::string tag_description, \
						int (*event_function)(CQmsg& msg), \
						int (*condition_function)(CQmsg& msg), \
						int trig_typ, \
						int event_typ, \
						int msg_codetyp, \
						std::string trigger_msg, \
						std::string trigger_reg) {

	this->tag.index = tag_index;
	this->tag.permission = tag_permission;
	this->tag.name = tag_name;
	this->tag.example = tag_example;
	this->tag.description = tag_description;
	this->event_func = event_function;
	this->custom_condition = condition_function;
	this->trig_type = trig_typ;
	this->event_type = event_typ;
	this->msg_codetype = msg_codetyp;
	if (!trigger_msg.empty()) {
		this->trig_msg.emplace_back(trigger_msg);
	}
	this->trig_regex = trigger_reg;
}

CQMsgEvent::~CQMsgEvent() {

}


int CQMsgEvent::trig(CQmsg& msg) {
	CQGroupMsg* p_group_msg;
	CQPrivateMsg* p_private_msg;
	switch (msg.msg_type) {
	case messageType::MSG_GROUP:
		p_group_msg = (CQGroupMsg*)&msg;
		if (this->event_type != eventType::EVENT_GROUP && this->event_type != eventType::EVENT_ALL) {
			return 0;
		}
		if (!this->isBannedGroup(p_group_msg->group->id)) {
			if (this->checkTriggerCondition(msg)) {
				return this->event_func(msg);
			}
			else {
				return 0;
			}
		}
		else {
			return 0;
		}
		break;

	case messageType::MSG_PRIVATE:
		p_private_msg = (CQPrivateMsg*)&msg;
		if (this->event_type != eventType::EVENT_PRIVATE && this->event_type != eventType::EVENT_ALL) {
			return 0;
		}
		if (!this->isBannedUser(p_private_msg->sender->id)) {
			if (this->checkTriggerCondition(msg)) {
				return this->event_func(msg);
			}
			else {
				return 0;
			}
		}
		else {
			return 0;
		}
		break;

	case messageType::MSG_UNDEFINED:
		return 0;
	default:
		return 0;
	}
}

int CQMsgEvent::checkTriggerCondition(CQmsg& msg) {
	const std::string* msg_str;
	std::regex reg;
	switch (this->msg_codetype) {
	case messageCodeType::CODE_UNICODE:
		msg_str = &msg.text();
		break;

	case messageCodeType::CODE_UTF8:
		msg_str = &msg.uniText();
		break;

	default:
		msg_str = &msg.uniText();
	}

	switch (this->trig_type) {
	case eventTriggerType::MSG_MATCH:
		for (auto iter = this->trig_msg.begin(); iter != this->trig_msg.end(); iter++) {
			if (*msg_str == *iter) {
				return 1;
			}
		}
		return 0;

	case eventTriggerType::MSG_CONTAIN:
		for (auto iter = this->trig_msg.begin(); iter != this->trig_msg.end(); iter++) {
			if ((*msg_str).find(*iter) != std::string::npos) {
				return 1;
			}
		}
		return 0;

	case eventTriggerType::MSG_REGEX:
		reg.assign(this->trig_regex);
		if (std::regex_match(*msg_str, reg)) {
			return 1;
		}
		else {
			return 0;
		}

	case eventTriggerType::TRIG_CUSTOMIZE:
		return this->custom_condition(msg);

	case eventTriggerType::TRIG_ALWAYS:
		return 1;

	case eventTriggerType::TRIG_NEVER:
		return 0;

	default:
		return 0;
	}
}