#ifndef TIME_EVENT_REGISTER_H
#define TIME_EVENT_REGIETER_H

#include "alpha_debug.h"
#include "cq_time_event.h"

typedef std::vector<CQTimeEvent>	timeEventList;
typedef std::vector<void*>			timeEventArgs;

class timeEventRegister {
public:
	timeEventRegister();
	~timeEventRegister();

	void append(void (*reg_func)(timeEventList&), void* arg);
	int registerEvents();
	int executeEvents(const aTime& time);
	size_t eventSize();
	int generateHelp(std::string& help_str,
		int max_permission = INT_MAX,
		int event_type = EVENT_ALL);
	timeEventList& getEvents();
private:
	timeEventList m_events;
	timeEventArgs m_args;
	std::vector<void(*)(timeEventList&)> m_regfuncs;
};

#endif // !TIME_EVENT_REGISTER_H
