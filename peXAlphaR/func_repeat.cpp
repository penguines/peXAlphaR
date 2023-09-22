#include "func_repeat.h"

int repeatMsg(CQmsg& msg){
	CQGroupMsg* group_msg;
	static Json::Value latestMsg;
	std::string msg_tmp, msg_text;
	int repeat_cnt = 0;
	int repeat_times = DEFAULT_REPEAT_TIMES;

	if (msg.msg_type != MSG_GROUP) {
		return 0;
	}

	group_msg = (CQGroupMsg*)&msg;
	Json::Value& group_data_json = group_msg->group->group_data.json;
	if (group_data_json.isMember("repeat")) {
		repeat_times = group_data_json["repeat"].asInt();
	}
	else {
		group_data_json["repeat"] = DEFAULT_REPEAT_TIMES;
		group_msg->group->group_data.save();
	}

	msg_text = group_msg->toStyledText();
	if (latestMsg.isMember(group_msg->group->id)) {
		loadStringByKeyword("message", latestMsg[group_msg->group->id], msg_tmp);
		if (msg_tmp == msg_text) {
			loadIntByKeyword("repeat", latestMsg[group_msg->group->id], repeat_cnt);
			repeat_cnt++;
			//Use == to repeat only once; >= & ["repeat"] = 0 for multi-repeat
			if (repeat_cnt == repeat_times) {
				sendGroupMsg(group_msg->group->id, group_msg->content);
				//latestMsg[group_msg->group->id]["repeat"] = 0;
				latestMsg[group_msg->group->id]["repeat"] = repeat_cnt;
			}
			else {
				latestMsg[group_msg->group->id]["repeat"] = repeat_cnt;
			}
			return repeat_cnt;
		}
		else {
			latestMsg[group_msg->group->id]["repeat"] = 1;
			latestMsg[group_msg->group->id]["message"] = msg_text;
			return 0;
		}
	}
	else {
		latestMsg[group_msg->group->id]["repeat"] = 1;
		latestMsg[group_msg->group->id]["message"] = msg_text;
		return 0;
	}
	return 0;
}

void register_repeat(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent repeat_event;
	repeat_event.event_func = repeatMsg;
	repeat_event.event_type = EVENT_GROUP;
	repeat_event.trig_type = TRIG_ALWAYS;
	repeat_event.msg_codetype = CODE_UTF8;
	repeat_event.tag.index = 0;
	repeat_event.tag.name = _G("复读");
	repeat_event.tag.permission = TRIG_REPEAT_PERMISSION_LEVEL;

	event_list.push_back(repeat_event);
}

int setRepeatTimes(CQmsg& msg){
	CQGroupMsg& group_msg = (CQGroupMsg&)msg;
	if (!checkPermissionLevel(group_msg, SET_REPEAT_PERMISSION_LEVEL)) {
		return 0;
	}

	std::regex reg("^/repeat\\s*([0-9]*)\\s*$");
	std::smatch reg_result;
	std::string msg_text = group_msg.text();
	std::regex_search(msg_text, reg_result, reg);
	msg_text = reg_result[1].str();
	group_msg.group->group_data.json["repeat"] = atoi(msg_text.c_str());
	int is_saved = group_msg.group->group_data.save();
	if (!is_saved) {
		sendGroupMsg(group_msg.group->id, "保存失败，未成功设置复读触发次数。", __IS_UTF8__);
		return 0;
	}
	else {
		sendGroupMsg(group_msg.group->id, "已成功设置复读触发次数为" + msg_text + "。", __IS_UTF8__);
		return 1;
	}
}

void register_setRepeatTimes(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = setRepeatTimes;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = "^/repeat\\s*([0-9]*)\\s*$";
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SET_REPEAT_PERMISSION_LEVEL;
	event_tmp.tag.name = _G("设置复读");
	event_tmp.tag.example = _G("/repeat [数字]");
	event_tmp.tag.description = _G("设置复读触发所需相同发言数。");

	event_list.push_back(event_tmp);
}
