#ifndef CQ_MSG_EVENT_H
#define CQ_MSG_EVENT_H

#include <regex>
#include "cq_event.h"

class CQMsgEvent : public CQEvent {
public:
	//descriptions of event, optional.
	eventTag tag;

	//pointer to function that event will trigger.
	int (*event_func)(CQmsg& msg);
	//Custom trigger condition function.Returning 1 for trig.
	int (*custom_condition)(CQmsg& msg);
	
	//using eventTriggerType
	int trig_type;
	//Code type of input CQmsg
	int msg_codetype;

	//default string as unicode
	std::vector<std::string> trig_msg;
	//trigger regex as string, available while trig_type is MSG_REGEX.
	std::string trig_regex;

	CQMsgEvent(int tag_index = 0,\
			int tag_permission = 0,\
			std::string tag_name = "",\
			std::string tag_example = "",\
			std::string tag_description = "",\
			int (*event_function)(CQmsg& msg) = nullptr,\
			int (*condition_function)(CQmsg& msg) = nullptr,\
			int trig_typ = eventTriggerType::TRIG_NEVER,\
			int event_typ = eventType::EVENT_NONE,\
			int msg_codetyp = messageCodeType::CODE_UTF8,\
			std::string trigger_msg = "",\
			std::string trigger_reg = "");

	~CQMsgEvent();

	//Check & trigger event
	int trig(CQmsg& msg);

private:
	int checkTriggerCondition(CQmsg& msg);
};

#endif