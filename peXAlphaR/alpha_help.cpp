#include "alpha_help.h"

static std::string owner_id;
static msgEventRegister* p_events_reg;

int getHelp(CQmsg& msg){
	if (p_events_reg == nullptr) {
		sendReply(msg, "��ǰ������δ�����κ�ָ�", __IS_UTF8__);
		return 0;
	}

	std::string help_text;
	CQGroupMsg* group_msg;
	CQPrivateMsg* private_msg;
	int group_permission;
	int private_permission;
	switch (msg.msg_type) {
	case MSG_GROUP:
		group_msg = (CQGroupMsg*)&msg;
		if (group_msg->sender->id == owner_id) {
			group_permission = OWNER_PERMISSION_LEVEL;
		}
		else if (checkIsOperator(*(group_msg->sender))) {
			group_permission = OP_PERMISSION_LEVEL;
		}
		else {
			group_permission = getGrpmbrPermissionLvl(*(group_msg->sender));
			if (!checkPermissionLevel(*group_msg, GET_HELP_PERMISSION_LEVEL)) {
				return 0;
			}
		}
		p_events_reg->generateHelp(help_text, group_permission, EVENT_GROUP);
		help_text = _G("����ǰ��ȺȨ�޵ȼ�Ϊ[") + std::to_string(group_permission) + _G("]����ʹ�����¹��ܣ�\n") + help_text;
		help_text.append(_G("ʹ�� /help [ָ����/ָ��] �ɻ�ȡ��һ��������"));
		sendGroupMsg(group_msg->group->id, help_text, 0);
		return 1;

	case MSG_PRIVATE:
		private_msg = (CQPrivateMsg*)&msg;
		if (checkIsBlacklisted(*(private_msg->sender))) {
			return 0;
		}
		if (private_msg->sender->id == owner_id) {
			private_permission = OWNER_PERMISSION_LEVEL;
		}
		else if (checkIsOperator(*(private_msg->sender))) {
			private_permission = OP_PERMISSION_LEVEL;
		}
		else{
			private_permission = 0;
		}
		p_events_reg->generateHelp(help_text, private_permission, EVENT_PRIVATE);
		help_text = _G("����ǰ��Ȩ�޵ȼ�Ϊ[") + std::to_string(private_permission) + _G("]����ʹ�����¹��ܣ�\n") + help_text;
		help_text.append(_G("ʹ�� /help [ָ����/ָ��] �ɻ�ȡ��һ��������"));
		sendPrivateMsg(private_msg->sender->id, help_text, 0);
		return 1;
	}
	return 0;
}

void register_getHelp(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = getHelp;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back("/help");
	event_tmp.trig_msg.emplace_back(_G("/����"));
	event_tmp.trig_msg.emplace_back(_G("#����"));
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = 0;
	event_tmp.tag.name = _G("����");
	event_tmp.tag.example = "/help";
	event_tmp.tag.description = _G("��ȡ������ʹ�ð�����");

	event_list.push_back(event_tmp);
}

int searchHelp(CQmsg& msg){
	if (!checkPermission(msg, GET_HELP_PERMISSION_LEVEL)) {
		return 0;
	}
	if (p_events_reg == nullptr) {
		sendReply(msg, "��ǰ������δ�����κ�ָ�", __IS_UTF8__);
		return 0;
	}

	std::regex reg(SEARCH_HELP_TRIG_REGEX);
	std::smatch reg_result;
	std::string msg_text = msg.uniText();
	std::regex_search(msg_text, reg_result, reg);
	msg_text = reg_result[2].str();

	msgEventList& events = p_events_reg->getEvents();
	std::vector<size_t> searched_events;
	std::string help_text;
	int is_matched = 0, is_contained = 0;
	for (size_t i = 0; i < events.size(); i++) {
		if (!events[i].tag.description.empty()) {
			if (events[i].tag.name == msg_text) {
				searched_events.emplace_back(i);
				is_matched = 1;
				break;
			}
		}
	}
	if (!is_matched) {
		for (size_t i = 0; i < events.size(); i++) {
			if (!events[i].tag.description.empty()) {
				if (events[i].tag.name.find(msg_text) != std::string::npos ||\
					events[i].tag.example.find(msg_text) != std::string::npos) {
					searched_events.emplace_back(i);
					is_contained = 1;
				}
			}
		}
	}
	if (is_matched || is_contained) {
		help_text.assign(_G("������")).append(std::to_string(searched_events.size())).append(_G("����ع���:\n"));
		for (auto iter = searched_events.begin(); iter != searched_events.end(); iter++) {
			help_text.append(events[*iter].tag.name).append(_G("(Ȩ�޵ȼ�[")).append(std::to_string(events[*iter].tag.permission));
			help_text.append("]):\n    ").append(events[*iter].tag.example).append(" -");
			help_text.append(events[*iter].tag.description).append("\n");
		}
		sendReply(msg, help_text, 0);
		return 1;
	}
	else {
		sendReply(msg, "δ��������ع��ܰ�����", __IS_UTF8__);
		return 0;
	}

	return 0;
}

void register_searchHelp(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = searchHelp;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = SEARCH_HELP_TRIG_REGEX;
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = 0;
	event_tmp.tag.name = _G("���Ұ���");
	event_tmp.tag.example = _G("/help(��/������#����) [�ַ���]");
	event_tmp.tag.description = _G("���һ�����ʹ�ð�����");

	event_list.push_back(event_tmp);
}

void alpha_help::setMsgEventRegister(msgEventRegister& event_reg){
	p_events_reg = &event_reg;
}

void alpha_help::setOwnerID(std::string& id){
	owner_id = id;
}
