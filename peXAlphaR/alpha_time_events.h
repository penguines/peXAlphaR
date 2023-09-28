#ifndef ALPHA_TIME_EVENTS_H
#define ALPHA_TIME_EVENTS_H

#include "cq_api_post.h"
#include "cq_time_event.h"
#include "alpha_runtime.h"

#define SETTIME_MENTION_PERMISSION	0

const std::string SETTIME_MENTION_PATH = "data/mentions.json";

typedef std::vector<aTime> aTimeList;

namespace alpha_timeEvents {
	void setImgFolder(const std::string& folder_path);
	void loadSetTimeMention(const std::string& folder_path);
}

int aTimeFromStr(const std::string time_str, aTime& _time);

int setTimeMention(const aTime& time, void* arg);
void register_setTimeMention(std::vector<CQTimeEvent>& event_list);

#endif // !ALPHA_TIME_EVENTS_H
