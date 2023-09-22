#include "alpha_permissions.h"

static std::string owner_id;
static jsonFileMap* users_data;

void alpha_permission::setBotOwner(std::string id) {
	owner_id = id;
}

void alpha_permission::setUserData(jsonFileMap& user_data){
	users_data = &user_data;
}

int checkIsOperator(CQuser& user) {
	int is_op = 0;
	if (user.customed_user_data != nullptr) {
		if (loadIntByKeyword("is_operator", user.customed_user_data->json, is_op)) {
			if (is_op) {
				return 1;
			}
			else {
				return 0;
			}
		}
		else {
			user.customed_user_data->json["is_operator"] = 0;
			user.customed_user_data->save();
			return 0;
		}
	}
	return 0;
}

int checkIsOperator(const CQuser& user){
	int is_op = 0;
	if (user.customed_user_data != nullptr) {
		if (loadIntByKeyword("is_operator", user.customed_user_data->json, is_op)) {
			if (is_op) {
				return 1;
			}
			else {
				return 0;
			}
		}
		else {
			return 0;
		}
	}
	return 0;
}

int checkIsBlacklisted(const CQuser& user){
	return 0;
}

int checkPermissionLevel(int user_permission_level, int required_permission_level, std::string& response_msg){
	if (user_permission_level >= required_permission_level) {
		response_msg = PERMISSION_ACCEPTED;
		return 1;
	}
	else {
		response_msg = PERMISSION_DENIED;
		return 0;
	}
}

int checkPermissionLevel(const CQGroupMsg& group_msg, int required_permission_level){
	int cur_permission_lvl = DEFAULT_PERMISSION_LEVEL;
	std::string response_msg;
	int check_result;
	const CQGroupMember& sender_tmp = *static_cast<const pCQGroupMember_t>(group_msg.sender);

	if (sender_tmp.id == owner_id) {
		return 1;
	}
	if (checkIsBlacklisted(sender_tmp)) {
		return 0;
	}
	if (checkIsOperator(sender_tmp)) {
		return 1;
	}

	cur_permission_lvl = getGrpmbrPermissionLvl(sender_tmp);
	if (cur_permission_lvl == -1) {
		sendGroupMsg(sender_tmp.id, PERMISSION_UNEXISTED, 0);
		return 0;
	}

	check_result = checkPermissionLevel(cur_permission_lvl, required_permission_level, response_msg);
	if (!response_msg.empty()) {
		sendGroupMsg(sender_tmp.id, response_msg, 0);
	}
	return check_result;
}

int checkPermission(const CQmsg& msg, int required_permission_level){
	switch (msg.msg_type) {
	case MSG_GROUP:
		return checkPermissionLevel(static_cast<const CQGroupMsg&>(msg), required_permission_level);
	case MSG_PRIVATE:
		if (checkIsBlacklisted(*static_cast<const CQPrivateMsg&>(msg).sender)) {
			return 0;
		}
		return 1;
	}
	return 0;
}

int getGrpmbrPermissionLvl(CQGroupMember& group_member) {
	int cur_permission_lvl = DEFAULT_PERMISSION_LEVEL;
	if (group_member.customed_grpmbr_data == nullptr) {
		return -1;
	}
	if (!loadIntByKeyword("permission", group_member.customed_grpmbr_data->json, cur_permission_lvl)) {
		group_member.customed_grpmbr_data->json["permission"] = DEFAULT_PERMISSION_LEVEL;
		group_member.customed_grpmbr_data->save();
	}
	cur_permission_lvl += group_member.permission_addition;
	return cur_permission_lvl;
}

int getGrpmbrPermissionLvl(const CQGroupMember& group_member){
	int cur_permission_lvl = DEFAULT_PERMISSION_LEVEL;
	if (group_member.customed_grpmbr_data == nullptr) {
		return -1;
	}
	loadIntByKeyword("permission", group_member.customed_grpmbr_data->json, cur_permission_lvl);
	cur_permission_lvl += group_member.permission_addition;
	return cur_permission_lvl;
}

int setOperator(CQmsg& msg){
	std::string request_id;
	switch (msg.msg_type) {
	case MSG_GROUP:
		request_id = ((CQGroupMsg*)&msg)->sender->id;
		break;
	case MSG_PRIVATE:
		request_id = ((CQPrivateMsg*)&msg)->sender->id;
		break;
	default:
		return 0;
	}
	if (request_id != owner_id) {
		return 0;
	}

	std::regex reg("^/op\\s*([0-9]*)\\s*$");
	std::smatch reg_result;
	std::string msg_text = msg.text();
	std::regex_search(msg_text, reg_result, reg);
	msg_text = reg_result[1].str();
	uint64_t uid = strtoull(msg_text.c_str(), NULL, 10);

	auto iter = users_data->find(uid);
	if(iter != users_data->end()){
		iter->second.json["is_operator"] = 1;
		iter->second.save();
		sendReply(msg, _G("已设置") + msg_text + _G("为管理员。"), 0);
		return 1;
	}
	sendReply(msg, "未找到用户，设置失败。", __IS_UTF8__);
	return 0;
}

void register_setOperator(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = setOperator;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = "^/op\\s*([0-9]*)\\s*$";
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SET_OP_PERMISSION_LEVEL;
	event_tmp.tag.name = _G("设置管理员");
	event_tmp.tag.example = _G("/op [账号]");
	event_tmp.tag.description = _G("添加新的机器人管理员(全局起效)。");

	event_list.push_back(event_tmp);
}

int delOperator(CQmsg& msg){
	std::string request_id;
	switch (msg.msg_type) {
	case MSG_GROUP:
		request_id = ((CQGroupMsg*)&msg)->sender->id;
		break;
	case MSG_PRIVATE:
		request_id = ((CQPrivateMsg*)&msg)->sender->id;
		break;
	default:
		return 0;
	}
	if (request_id != owner_id) {
		return 0;
	}

	std::regex reg("^/deop\\s*([0-9]*)\\s*$");
	std::smatch reg_result;
	std::string msg_text = msg.text();
	std::regex_search(msg_text, reg_result, reg);
	msg_text = reg_result[1].str();
	uint64_t uid = strtoull(msg_text.c_str(), NULL, 10);

	auto iter = users_data->find(uid);
	if (iter != users_data->end()) {
		iter->second.json["is_operator"] = 0;
		iter->second.save();
		sendReply(msg, msg_text + _G("不再是管理员。"), 0);
		return 1;
	}
	sendReply(msg, "未找到用户，设置失败。", __IS_UTF8__);
	return 0;
}

void register_delOperator(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = delOperator;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = "^/deop\\s*([0-9]*)\\s*$";
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SET_OP_PERMISSION_LEVEL;
	event_tmp.tag.name = _G("删除管理员");
	event_tmp.tag.example = _G("/deop [账号]");
	event_tmp.tag.description = _G("移除现有的机器人管理员(全局起效)。");

	event_list.push_back(event_tmp);
}
