#include "alpha_time_events.h"

/*
Structure:
[
	{
		"time": ["y/m/d weekday h:m:s", "time2", ...],
		#white list
		"group_id": [114514, 1919810, ...],
		#black list
		"group_id": [0, -114514, -1919810, ...],
		"message": ["msg1", "msg2", ...]
	}
]
*/
JsonFile_t settime_mentions;
std::vector<aTimeList> settime_times;

int aTimeFromStr(const std::string time_str, aTime& _time){
	int year, month, mday, wday, h, m, s;
	int res;
	res = sscanf(time_str.c_str(), "%d/%d/%d %d %d:%d:%d", \
		&year, &month, &mday, &wday, &h, &m, &s);
	_time.setYear(year);
	_time.setMonth(month);
	_time.setMDay(mday);
	_time.setWDay(wday);
	_time.setHour(h);
	_time.setMin(m);
	_time.setSec(s);
	return res;
}

int setTimeMention(const aTime& time, void* arg){
	const Json::Value& mentions = settime_mentions.json;
	if (!mentions.isArray()) {
		return 0;
	}
	for (int i = 0; i < mentions.size(); i++) {
		for (auto tm_it = settime_times[i].begin(); tm_it != settime_times[i].end(); tm_it++) {
			if (setTimeCompare(time, *tm_it) == 0) {

				break;
			}
		}
	}

    return 1;
}

void register_setTimeMention(std::vector<CQTimeEvent>& event_list){
	CQTimeEvent event_tmp;
	event_tmp.event_func = setTimeMention;

	event_tmp.event_type = eventType::EVENT_ALL;
	event_tmp.trig_type = eventTriggerType::TRIG_ALWAYS;
	event_tmp.exec_type = timeEventExec::EXEC_DETACH;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SETTIME_MENTION_PERMISSION;
	event_tmp.tag.name = _G("定时提醒");
	event_tmp.tag.example = _G("[数据删除]");
	event_tmp.tag.description = _G("定时提醒。");

	event_list.push_back(event_tmp);
}
