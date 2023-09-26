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

void alpha_timeEvents::loadSetTimeMention(const std::string& folder_path) {
	uint32_t cnt = 0;
	settime_mentions.file_path = folder_path;
	settime_mentions.file_name = SETTIME_MENTION_PATH;
	if (settime_mentions.load()) {
		const Json::Value& mentions = settime_mentions.json;
		if (!mentions.isArray()) {
			PRINTLOG("[定时提醒]目前暂无已配置的定时提醒。");
			return;
		}
		for (auto iter = mentions.begin(); iter != mentions.end(); iter++) {
			const Json::Value& times = (*iter)["time"];
			if (!times.isArray()) {
				return;
			}
			settime_times.emplace_back();
			aTimeList& timelist_tmp = settime_times.back();
			for (auto tm_it = times.begin(); tm_it != times.end(); tm_it++) {
				timelist_tmp.emplace_back();
				aTimeFromStr(tm_it->asString(), timelist_tmp.back());
			}
			cnt++;
		}
		PRINTLOG("[定时提醒]已配置%d条定时提醒。", cnt);
	}
	else {
		PRINTLOG("[定时提醒]未找到定时提醒配置文件。");
	}
}

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
	CQGroupList* grp_data = static_cast<CQGroupList*>(arg);
	const Json::Value& mentions = settime_mentions.json;
	CQJsonMsg msg;
	uint32_t msg_cnt = 0;

	if (!mentions.isArray()) {
		return 0;
	}
	for (int i = 0; i < settime_times.size(); i++) {
		for (auto tm_it = settime_times[i].begin(); tm_it != settime_times[i].end(); tm_it++) {
			if (setTimeCompare(time, *tm_it) == 0) {
				const Json::Value& grp_list = mentions[i]["group_id"];
				const Json::Value& msg_list = mentions[i]["message"];
				bool blacklist_mode = false;
				std::vector<uint64_t> blacklist;

				if (!grp_list.isArray()) {
					return 0;
				}
				if (!msg_list.isArray()) {
					return 0;
				}
				for (int j = 0; j < grp_list.size(); j++) {
					//int64_id > 0: white list mode
					int64_t grp_id = grp_list[j].asInt64();
					if (grp_id > 0) {
						if (grp_data->find(grp_id) == grp_data->end()) {
							continue;
						}
						if (isGroupAvailable(&(*grp_data)[grp_id])) {
							msg.fromStyledText(msg_list[randomInt(0, msg_list.size() - 1)].asString());
							sendGroupMsg((*grp_data)[grp_id].id, msg.getJson());
							msg_cnt++;
							PRINTLOG("[定时提醒]在群%s中发出一条定时提醒。", (*grp_data)[grp_id].id.c_str());
						}
					}
					else if (grp_id == 0) {
						blacklist_mode = true;
					}
					else {
						grp_id = -grp_id;
						if (grp_data->find(grp_id) != grp_data->end()) {
							blacklist.emplace_back(grp_id);
						}
					}
				}
				if (blacklist_mode) {
					for (auto grp_it = grp_data->begin(); grp_it != grp_data->end(); grp_it++) {
						if (!searchFromVector(blacklist, grp_it->second.u64_id)) {
							if (isGroupAvailable(&(grp_it->second))) {
								msg.fromStyledText(msg_list[randomInt(0, msg_list.size() - 1)].asString());
								sendGroupMsg(grp_it->second.id, msg.getJson());
								msg_cnt++;
								PRINTLOG("[定时提醒]在群%s中发出一条定时提醒。", grp_it->second.id.c_str());
							}
						}
					}
				}
				break;
			}
		}
	}

    return msg_cnt;
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
