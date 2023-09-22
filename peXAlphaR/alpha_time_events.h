#ifndef ALPHA_TIME_EVENTS_H
#define ALPHA_TIME_EVENTS_H

#include "cq_api_post.h"
#include "cq_time_event.h"

#define SETTIME_MENTION_PERMISSION	0

int setTimeMention(const aTime& time, void* arg);
void register_setTimeMention(std::vector<CQTimeEvent>& event_list);

#endif // !ALPHA_TIME_EVENTS_H