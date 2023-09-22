#include "alpha_inits.h"

void initConfig(std::string& folder_path , jsonFile& config_json, alphaConfig& config){
	config_json.file_path = folder_path;
	config_json.file_name = CONFIG_FILE_NAME;
	config_json.identifier = "config";
	config_json.load();
	//此处无需考虑load()是否成功，若读取成功则loadString/loadInt可将配置读入;
	//若读取不成功则会将默认配置写入并储存。
	loadStringByKeyword("owner_id", config_json.json, config.owner_id, "");
	loadStringByKeyword("sauce_apikey", config_json.json, config.sauce_apikey, "");
	//load and replace '\\' to '/'
	loadStringByKeyword("gocq_folder", config_json.json, config.gocq_folder, "");
	if (replaceAllChar(config.gocq_folder, '\\', '/')) {
		config_json.json["gocq_folder"] = config.gocq_folder;
	}
	loadStringByKeyword("recv_host", config_json.json, config.recv_host, DEFAULT_RECV_HOST);
	loadStringByKeyword("post_host", config_json.json, config.post_host, DEFAULT_POST_HOST);
	loadIntByKeyword("recv_port", config_json.json, config.recv_port, DEFAULT_RECV_PORT);
	loadIntByKeyword("post_port", config_json.json, config.post_port, DEFAULT_POST_PORT);
	config_json.save();
}

// 机器人文件夹\\data\\groups\\群号\\global.json
//								\\[user_id].json
void initGroupsData(std::string folder_path, CQGroupList& groups, CQGrpMbrDataList& group_members) {
	uint64_t uid;
	std::string str_id;
	std::vector<std::string> sub_folders, member_files;
	getSubFolders(folder_path, sub_folders);
	for (int i = 0; i < sub_folders.size(); i++) {
		uid = strtoull(sub_folders[i].c_str(), NULL, 10);
		groups.insert(CQGroupList::value_type(uid, CQgroup()));
		CQgroup& group_tmp = groups[uid];
		group_tmp.id = sub_folders[i];
		group_tmp.u64_id = uid;
		jsonFile& json_file_tmp = group_tmp.group_data;
		json_file_tmp.file_path = folder_path + sub_folders[i] + "\\";
		json_file_tmp.file_name = GROUP_DATA_FILE_NAME;
		json_file_tmp.identifier = sub_folders[i];
		json_file_tmp.uid = uid;
		json_file_tmp.load();
		if (initGroupJson(json_file_tmp.json) > 0) {
			json_file_tmp.save();
		}

		//Load group members data for each group.
		getFilesB(json_file_tmp.file_path, member_files);
		group_members.insert(CQGrpMbrDataList::value_type(uid, jsonFileMap()));
		jsonFileMap& grpmbr_tmp = group_members[uid];
		for (int j = 0; j < member_files.size(); j++) {
			if (member_files[j] != GROUP_DATA_FILE_NAME) {
				getFileName(member_files[j], str_id);
				//reload uid as user_id
				uid = strtoull(str_id.c_str(), NULL, 10);
				grpmbr_tmp.insert(jsonFileMap::value_type(uid, jsonFile()));
				jsonFile& mbr_data_tmp = grpmbr_tmp[uid];
				mbr_data_tmp.file_path = json_file_tmp.file_path;
				mbr_data_tmp.file_name = member_files[j];
				mbr_data_tmp.identifier = str_id;
				mbr_data_tmp.uid = uid;
				mbr_data_tmp.load();
				if (initGroupMemberJson(mbr_data_tmp.json) > 0) {
					mbr_data_tmp.save();
				}
			}
		}
		member_files.clear();
	}
}

// 机器人文件夹\\data\\users\\[user_id].json

void initUsersData(std::string folder_path, CQUserDataList& users_data){
	std::string str_id;
	uint64_t uid;
	std::vector<std::string> user_files;
	getFilesB(folder_path, user_files);
	for (int i = 0; i < user_files.size(); i++) {
		getFileName(user_files[i],	str_id);
		uid = strtoull(str_id.c_str(), NULL, 10);
		users_data.insert(CQUserDataList::value_type(uid, jsonFile(folder_path, user_files[i], str_id, uid)));
		jsonFile& json_file_tmp = users_data[uid];
		json_file_tmp.load();
		if (initUserJson(json_file_tmp.json) > 0) {
			json_file_tmp.save();
		}
	}
}

void initActiveUsers(const CQGrpMbrDataList& group_members, CQUserDataList& users_data, jsonFileGroup& active_users){
	uint64_t u64_group_id;
	int signs = 0;
	for (auto iter = group_members.begin(); iter != group_members.end(); iter++) {
		u64_group_id = iter->first;
		active_users.insert(jsonFileGrpUnit(u64_group_id, pJsonFileList()));
		pJsonFileList& pjson_tmp = active_users.find(u64_group_id)->second;
		const jsonFileMap& members = iter->second;
		for (auto mbr = members.begin(); mbr != members.end(); mbr++) {
			auto cur_user = users_data.find(mbr->first);
			if (cur_user == users_data.end()) {
				continue;
			}
			signs = 0;
			loadIntByKeyword("cur_signs", cur_user->second.json, signs);
			if (signs > 0) {
				pjson_tmp.emplace_back(&(cur_user->second));
			}
		}
		//DEBUGLOG("Group %llu loaded %d active users.", u64_group_id, pjson_tmp.size());
	}
}

int initGroupJson(Json::Value& group_json){
	int cnt = 0;
	cnt += setEmptyJson(group_json, "repeat", DEFAULT_REPEAT_TIMES);
	cnt += setEmptyJson(group_json, "available", 0);
	return cnt;
}

int initUserJson(Json::Value& user_json){
	int cnt = 0;
	cnt += setEmptyJson(user_json, "cur_signs", 0);
	cnt += setEmptyJson(user_json, "cur_continuous_signs", 0);
	cnt += setEmptyJson(user_json, "max_continuous_signs", 0);
	cnt += setEmptyJson(user_json, "present_draw", 0);
	cnt += setEmptyJson(user_json, "favor", 0);
	cnt += setEmptyJson(user_json, "is_operator", 0);
	return cnt;
}

int initGroupMemberJson(Json::Value& grpmbr_json){
	int cnt = 0;
	cnt += setEmptyJson(grpmbr_json, "permission", DEFAULT_PERMISSION_LEVEL);
	return cnt;
}

int replaceAllChar(std::string& str, char src, char dst){
	int cnt = 0;
	for (auto iter = str.begin(); iter != str.end(); iter++) {
		if (*iter == src) {
			*iter = dst;
			cnt++;
		}
	}
	return cnt;
}

