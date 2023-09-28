#include "at_reply.h"

JsonFile_t at_replies;
static std::string img_folder_path;

void alpha_atReply::setImgFolder(const std::string& folder_path) {
	img_folder_path = folder_path;
}

void alpha_atReply::loadRepliesFile(const std::string& folder_path){
	at_replies.file_path = folder_path;
	at_replies.file_name = AT_REPLIES_PATH;
	at_replies.load();
}

int atReply(CQmsg& msg){
	std::string msg_text(msg.text()), msg_text_lc;
	const Json::Value& replies = at_replies.json;
	CQJsonMsg reply_msg;

	CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
	uint64_t& grp_id = grp_msg.group->u64_id;

	while (isspace(msg_text.at(0))) {
		msg_text.erase(0, 1);
		if (msg_text.empty()) {
			break;
		}
	}

	if (!replies.isArray()) {
		return 0;
	}
	for (int i = 0; i < replies.size(); i++) {
		uint64_t grpid_tmp = 1;
		loadUInt64ByKeyword("group_id", replies[i], grpid_tmp);
		if (grp_id == grpid_tmp || grpid_tmp == 0) {
			const Json::Value& rep_tmp = replies[i]["reply"];
			if (!rep_tmp.isArray()) {
				return 0;
			}
			for (int j = 0; j < rep_tmp.size(); j++) {
				//if(!rep_tmp.isMember("content")) return 0;
				std::string msg_match;
				int match_type;
				loadIntByKeyword("match_type", rep_tmp[j], match_type);
				if (match_type & MATCH_LCASE_MASK) {
					if (msg_text_lc.empty()) {
						msg_text_lc.assign(msg_text);
						toLowerCase(msg_text_lc);
					}
					msg_match = msg_text_lc;
				}
				else {
					msg_match = msg_text;
				}

				bool is_match = false;
				const Json::Value& match_arr = rep_tmp[j]["content"];
				if (!match_arr.isArray()) {
					return 0;
				}
				if ((match_type & MATCH_TYPE_MASK) == 0) {
					for (auto iter = match_arr.begin(); iter != match_arr.end(); iter++) {
						if (msg_match == iter->asString()) {
							is_match = true;
							break;
						}
					}
				}
				else {
					for (auto iter = match_arr.begin(); iter != match_arr.end(); iter++) {
						if (msg_match.find(iter->asString()) != std::string::npos) {
							is_match = true;
							break;
						}
					}
				}
				if (is_match) {
					//if(!rep_tmp.isMember("reply")) return 0;
					const Json::Value& rep_arr = rep_tmp[j]["reply"];
					if (!rep_arr.isArray()) {
						return 0;
					}
					reply_msg.fromStyledText(rep_arr[randomInt(0, rep_arr.size() - 1)].asString());
					reply_msg.replaceSpecialAt(grp_msg.sender->id, REPLACE_AT_SENDER);
					reply_msg.fillImgPath(img_folder_path);

					sendGroupMsg(grp_msg.group->id, reply_msg.getJson());
					return 1;
				}
			}
			break;
		}
	}

    return 0;
}

int isAtBot(CQmsg& msg){
	std::string str_tmp;
	if (!msg.content.isArray()) {
		return 0;
	}
	for (int i = 0; i < msg.content.size(); i++) {
		loadStringByKeyword("type", msg.content[i], str_tmp);
		if (str_tmp == "at") {
			if (msg.content[i].isMember("data")) {
				loadStringByKeyword("qq", msg.content[i]["data"], str_tmp);
				if (str_tmp == msg.self_id) {
					return 1;
				}
			}
		}
	}
	return 0;
}

void register_atReply(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = atReply;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.custom_condition = isAtBot;
	event_tmp.trig_type = TRIG_CUSTOMIZE;
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.name = _G("自动回复");
	event_tmp.tag.permission = AT_REPLY_PERMISSION;
	event_tmp.tag.example = _G("[数据删除]");
	event_tmp.tag.description = _G("自动回复");

	event_list.push_back(event_tmp);
}

int atReplyReload(CQmsg& msg){
	if (at_replies.load()) {
		sendReply(msg, _U("成功载入自动回复配置！"), 1);
		return 1;
	}
	else {
		sendReply(msg, _U("自动回复配置载入失败。"), 1);
		return 0;
	}
}

void register_atReplyReload(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = atReplyReload;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.trig_msg.emplace_back("/autoreply reload");
	event_tmp.tag.index = 0;
	event_tmp.tag.name = _G("自动回复配置重载");
	event_tmp.tag.permission = ATREPLY_RELOAD_PERMISSION;
	event_tmp.tag.example = _G("/autoreply reload");
	event_tmp.tag.description = _G("热重载自动回复配置。");

	event_list.push_back(event_tmp);
}
