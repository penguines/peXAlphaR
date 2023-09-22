#ifndef ALPHA_INITS_H
#define ALPHA_INITS_H

#include <string>
#include <vector>
#include "alpha_config.h"
#include "alpha_paths.h"
#include "alpha_permissions.h"
#include "json_file.h"
#include "cq_group.h"
#include "cq_api_post.h"
#include "func_repeat.h"

#define GROUP_DATA_FILE_NAME "global.json"

const std::string CONFIG_FILE_NAME = "config.json";
const std::string GROUP_DATA_PATH = "data\\groups\\";
const std::string USER_DATA_PATH = "data\\users\\";
const std::string IMG_FILES_PATH = "img\\";

//typedef jsonFileList CQUserDataList;
typedef jsonFileMap CQUserDataList;
typedef std::unordered_map<uint64_t, jsonFileMap> CQGrpMbrDataList;

//init config
void initConfig(std::string& folder_path, jsonFile& config_json, alphaConfig& config);
//init groups and group members
void initGroupsData(std::string folder_path, CQGroupList& groups, CQGrpMbrDataList& group_members);
//init users
void initUsersData(std::string folder_path, CQUserDataList& users_data);
//init active users for each group
void initActiveUsers(const CQGrpMbrDataList& group_members, 
					CQUserDataList& users_data,  
					jsonFileGroup& active_users);

int initGroupJson(Json::Value& group_json);
int initUserJson(Json::Value& user_json);
int initGroupMemberJson(Json::Value& grpmbr_json);
//return number of replaced chars.
int replaceAllChar(std::string& str, char src, char dst);

#endif // !ALPHA_INITS_H
