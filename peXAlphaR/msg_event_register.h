#ifndef MSG_EVENT_REGISTER_H
#define MSG_EVENT_REGISTER_H

#include "alpha_debug.h"

#include <vector>
#include "cq_msg_event.h"

typedef std::vector<CQMsgEvent> msgEventList;
typedef std::vector<void(*)(msgEventList&)> registerFuncList;

class msgEventRegister {
public:
	msgEventRegister();
	~msgEventRegister();

	void append(void (*reg_func)(msgEventList&));
	int registerEvents();
	int executeEvents(CQmsg& msg);
	size_t eventSize();
	int generateHelp(std::string& help_str,
					 int max_permission = INT_MAX,
					 int event_type = EVENT_ALL);
	msgEventList& getEvents();
private:
	msgEventList m_events;
	registerFuncList m_regfuncs;
};

#endif // !MSG_EVENT_REGISTER_H
