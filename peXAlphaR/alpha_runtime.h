#ifndef ALPHA_RUNTIME_H
#define ALPHA_RUNTIME_H

//��������Ա�֤�ڴ������û�����ʧ�ܵ�����³����Ի����յ�jsonFile����
//�Ա���Json::Value����null.
#define ALPHA_NO_NULL_JSON

#include <string>
#include <vector>
#include <random>
#include "cq_msg.h"
#include "json_file.h"
#include "alpha_inits.h"
#include "alpha_permissions.h"

#define ALPHA_START_PERMISSION_LEVEL 3
#define ALPHA_START_MSG			"Type-Alpha������."
#define ALPHA_STOP_MSG			"Type-Alpha��ͣ��."
#define ALPHA_ALREADY_STARTED	"Type-Alpha��������."

namespace alpha_runtime {
	//necessary for createNewData functions.
	void setGroupDataFolder(std::string folder_path);
	void setUserDataFolder(std::string folder_path);
};

void loadCQUserData(CQuser& user, CQUserDataList& users_data);
void loadCQmsgUserData(CQPrivateMsg& msg, CQUserDataList& user_data);
void loadCQmsgUserData(CQGroupMsg& msg, CQUserDataList& user_data);
void loadCQmsgGroupData(CQGroupMsg& msg, CQGroupList& groups_data, CQGrpMbrDataList& groups_member_data);
void createNewCQUserData(CQUserDataList& users_data, const std::string& user_id);
void createNewCQGrpmbrData(jsonFileMap& member_data, const std::string& group_id, const std::string& user_id);
int createNewCQGroupData(CQGroupList& group_list, const std::string& group_id);

int isGroupAvailable(pCQgroup_t group);
int isGroupAvailable(CQGroupMsg& group_msg);
int checkGroupStart(CQGroupMsg& group_msg);

//[lb, ub]
int randomInt(int lb, int ub);
//[lb, ub)
double randomDouble(double lb, double ub);

void register_enableGroup(std::vector<CQMsgEvent>& event_list);

int disableGroup(CQmsg& msg);
void register_disableGroup(std::vector<CQMsgEvent>& event_list);

#endif // !ALPHA_RUNTIME_H
