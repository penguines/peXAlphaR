#include "alpha_runtime.h"

static std::string GROUP_DATA_FOLDER;
static std::string USER_DATA_FOLDER;
static jsonFile EMPTY_JSON_DATA;

void alpha_runtime::setGroupDataFolder(std::string folder_path){
	GROUP_DATA_FOLDER = folder_path;
}

void alpha_runtime::setUserDataFolder(std::string folder_path){
	USER_DATA_FOLDER = folder_path;
}

void loadCQUserData(CQuser& user, CQUserDataList& users_data){
	auto cur_user = users_data.find(user.u64_id);
	if (cur_user != users_data.end()) {
		user.customed_user_data = &(cur_user->second);
		return;
	}
	if (!USER_DATA_FOLDER.empty()) {
		createNewCQUserData(users_data, user.id);
		user.customed_user_data = &users_data[user.u64_id];
		return;
	}

#ifdef ALPHA_NO_NULL_JSON
	//防止未定义数据存储文件夹导致pJsonFile_t为nullptr引发错误
	user.customed_user_data = &EMPTY_JSON_DATA;
#endif // ALPHA_NO_NULL_JSON
}

void loadCQmsgUserData(CQPrivateMsg& msg, CQUserDataList& user_data){
	loadCQUserData(*(msg.sender), user_data);
}

void loadCQmsgUserData(CQGroupMsg& msg, CQUserDataList& user_data){
	loadCQUserData(*(msg.sender), user_data);
}

void loadCQmsgGroupData(CQGroupMsg& msg, CQGroupList& groups_data, CQGrpMbrDataList& groups_member_data){
	auto cur_group = groups_data.find(msg.group->u64_id);
	if (cur_group != groups_data.end()) {
		msg.group = &(cur_group->second);
		jsonFileMap& members_tmp = groups_member_data[msg.group->u64_id];
		auto member = members_tmp.find(msg.sender->u64_id);
		if(member != members_tmp.end()){
			msg.sender->customed_grpmbr_data = &(member->second);
			return;
		}
		createNewCQGrpmbrData(members_tmp, msg.group->id, msg.sender->id);
		msg.sender->customed_grpmbr_data = &(members_tmp[msg.sender->u64_id]);
		return;
	}
	if (!GROUP_DATA_FOLDER.empty()) {
		if(createNewCQGroupData(groups_data, msg.group->id)){
			msg.group = &(groups_data[msg.group->u64_id]);
			groups_member_data.insert(CQGrpMbrDataList::value_type(msg.group->u64_id, jsonFileMap()));
			jsonFileMap& grpmbr_tmp = groups_member_data[msg.group->u64_id];
			createNewCQGrpmbrData(grpmbr_tmp, msg.group->id, msg.sender->id);
			msg.sender->customed_grpmbr_data = &(grpmbr_tmp[msg.sender->u64_id]);
			return;
		}
	}

#ifdef ALPHA_NO_NULL_JSON
	//防止未定义数据存储文件夹导致pJsonFile_t为nullptr引发错误
	msg.sender->customed_grpmbr_data = &EMPTY_JSON_DATA;
#endif // ALPHA_NO_NULL_JSON
}

void createNewCQUserData(CQUserDataList& users_data, const std::string& user_id){
	uint64_t uid = strtoull(user_id.c_str(), NULL, 10);
	users_data.insert(CQUserDataList::value_type(uid, jsonFile()));
	jsonFile& new_user_data = users_data[uid];
	new_user_data.file_path = USER_DATA_FOLDER;
	new_user_data.file_name = user_id + ".json";
	new_user_data.identifier = user_id;
	new_user_data.json["user_id"] = user_id;
	//customed data
	initUserJson(new_user_data.json);
	new_user_data.save();
}

void createNewCQGrpmbrData(jsonFileMap& group_member_data, const std::string& group_id, const std::string& user_id){
	uint64_t uid = strtoull(user_id.c_str(), NULL, 10);
	group_member_data.insert(jsonFileMap::value_type(uid, jsonFile()));
	jsonFile& new_member_data = group_member_data[uid];
	new_member_data.file_path.assign(GROUP_DATA_FOLDER).append(group_id).append("\\");
	new_member_data.file_name.assign(user_id).append(".json");
	new_member_data.identifier = user_id;
	new_member_data.json["user_id"] = user_id;
	//customed data
	initGroupMemberJson(new_member_data.json);
	new_member_data.save();
}

int createNewCQGroupData(CQGroupList& group_list, const std::string& group_id){
	uint64_t uid = strtoull(group_id.c_str(), NULL, 10);
	if (createFolder(GROUP_DATA_FOLDER + group_id + "\\") == 0) {
		group_list.insert(CQGroupList::value_type(uid, CQgroup()));
		CQgroup& group_tmp = group_list[uid];
		group_tmp.id = group_id;
		group_tmp.u64_id = uid;
		jsonFile& new_group_data = group_tmp.group_data;
		new_group_data.file_path = GROUP_DATA_FOLDER + group_id + "\\";
		new_group_data.file_name = GROUP_DATA_FILE_NAME;
		new_group_data.identifier = group_id;
		new_group_data.json["group_id"] = group_id;
		//customed data
		initGroupJson(new_group_data.json);
		new_group_data.save();
		return 1;
	}
	else {
		return 0;
	}
}

int isGroupAvailable(pCQgroup_t group){
	int is_ava = 0;
	if (!loadIntByKeyword("available", group->group_data.json, is_ava)) {
		group->group_data.json["available"] = 0;
		group->group_data.save();
		return 0;
	}
	return is_ava;
}

int isGroupAvailable(CQGroupMsg& group_msg){
	return isGroupAvailable(group_msg.group);
}

int checkGroupStart(CQGroupMsg& group_msg){
	std::string msg_text = group_msg.uniText();
	if (msg_text == "/start" || msg_text == _G("/启动")) {
		if (!checkPermissionLevel(group_msg, ALPHA_START_PERMISSION_LEVEL)) {
			return 0;
		}
		if (!isGroupAvailable(group_msg)) {
			group_msg.group->group_data.json["available"] = 1;
			group_msg.group->group_data.save();
			sendGroupMsg(group_msg.group->id, ALPHA_START_MSG, 0);
			return 1;
		}
		else {
			sendGroupMsg(group_msg.group->id, ALPHA_ALREADY_STARTED, 0);
			return 0;
		}
	}
	return 0;
}

int randomInt(int lb, int ub){
	static std::default_random_engine rand_engine;
	static bool is_init = false;
	std::uniform_int_distribution<int> rand_int(lb, ub);
	if (!is_init) {
		rand_engine.seed(time(0));
		is_init = true;
	}

	return rand_int(rand_engine);
}

double randomDouble(double lb, double ub){
	static std::default_random_engine rand_engine;
	static bool is_init = false;
	std::uniform_real_distribution<double> rand_dbl(lb, ub);
	if (!is_init) {
		rand_engine.seed(time(0));
		is_init = true;
	}

	return rand_dbl(rand_engine);
}

void register_enableGroup(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	//refer to checkGroupStart, but needless for eventRegister to execute;
	//thus set trig_type to TRIG_NEVER.
	event_tmp.event_func = nullptr;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.trig_type = TRIG_NEVER;
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = ALPHA_START_PERMISSION_LEVEL;
	event_tmp.tag.name = _G("启动机器人");
	event_tmp.tag.example = _G("/start(/启动)");
	event_tmp.tag.description = _G("在当前群聊中启用机器人。");

	event_list.push_back(event_tmp);
}

int disableGroup(CQmsg& msg){
	int is_ava = 0;
	CQGroupMsg& group_msg = (CQGroupMsg&)msg;
	if (!checkPermissionLevel(group_msg, ALPHA_START_PERMISSION_LEVEL)) {
		return 0;
	}
	group_msg.group->group_data.json["available"] = 0;
	group_msg.group->group_data.save();
	sendGroupMsg(group_msg.group->id, ALPHA_STOP_MSG, 0);
	return 1;
}

void register_disableGroup(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = disableGroup;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back("/stop");
	event_tmp.trig_msg.emplace_back(_G("/停止"));
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = ALPHA_START_PERMISSION_LEVEL;
	event_tmp.tag.name = _G("关闭机器人");
	event_tmp.tag.example = _G("/stop(/停止)");
	event_tmp.tag.description = _G("在当前群聊中关闭机器人。");

	event_list.push_back(event_tmp);
}
