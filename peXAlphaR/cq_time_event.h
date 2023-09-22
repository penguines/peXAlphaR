#ifndef CQ_TIME_EVENT_H
#define CQ_TIME_EVENT_H

#include <thread>
#include "alpha_debug.h"
#include "cq_event.h"
#include "alpha_time.h"

typedef enum timeEventExec { EXEC_JOIN = 0, EXEC_DETACH = 1 };

typedef struct aPeriod{
	aTime begin;
	aTime end;
} aPeriod;

class CQTimeEvent : public CQEvent {
public:
	//using eventTriggerType
	int trig_type;
	//execution type
	int exec_type;
	
	//pointer to function that event will trigger.
	int (*event_func)(const aTime& time, void* arg);
	//Custom trigger condition function.Returning 1 for trig.
	int (*custom_condition)(const aTime& time, void* arg);

	std::vector<aTime> trig_time;
	std::vector<aPeriod> trig_period;

	CQTimeEvent(int tag_index = 0, \
				int tag_permission = 0, \
				std::string tag_name = "", \
				std::string tag_example = "", \
				std::string tag_description = "", \
				int (*event_function)(const aTime& time, void* arg) = nullptr, \
				int (*condition_function)(const aTime& time, void* arg) = nullptr, \
				int trig_typ = eventTriggerType::TRIG_NEVER, \
				int event_typ = eventType::EVENT_NONE, \
				int exec_typ = timeEventExec::EXEC_JOIN);
	~CQTimeEvent();

	//Check & trigger event
	int trig(const aTime& time, void* arg);

private:
	int checkTriggerCondition(const aTime& time, void* arg);
};

int setTimeCompare(const aTime& current_time, const aTime& set_time);

#endif // !CQ_TIME_EVENT_H
