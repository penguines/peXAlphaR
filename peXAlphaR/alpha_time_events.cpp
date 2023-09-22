#include "alpha_time_events.h"

int setTimeMention(const aTime& time, void* arg){
    sendGroupMsg("109336271", _G("测试"));
    return 1;
}

void register_setTimeMention(std::vector<CQTimeEvent>& event_list){
	CQTimeEvent event_tmp;
	aTime trig_tm(-1, -1, -1, -1, 17, 25, 10, 8);
	event_tmp.event_func = setTimeMention;

	event_tmp.event_type = eventType::EVENT_ALL;
	event_tmp.trig_type = eventTriggerType::TIME_MATCH;
	event_tmp.exec_type = timeEventExec::EXEC_DETACH;
	event_tmp.trig_time.emplace_back(trig_tm);
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SETTIME_MENTION_PERMISSION;
	event_tmp.tag.name = _G("定时提醒");
	event_tmp.tag.example = _G("[数据删除]");
	event_tmp.tag.description = _G("定时提醒。");

	event_list.push_back(event_tmp);
}
