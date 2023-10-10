#ifndef ALPHA_PERMISSIONS_H
#define ALPHA_PERMISSIONS_H

#include <string>
#include "cq_api_post.h"
#include "cq_msg.h"
#include "cq_msg_event.h"
#include "json_file.h"

#define NORMAL_PERMISSION_LEVEL		0
#define DEFAULT_PERMISSION_LEVEL	1
#define OWNER_PERMISSION_LEVEL		INT_MAX
#define OP_PERMISSION_LEVEL			INT_MAX - 1
#define SET_OP_PERMISSION_LEVEL		INT_MAX

#define PERMISSION_ACCEPTED ""
#define PERMISSION_DENIED	"您的权限不足以使用本命令。"
#define PERMISSION_UNEXISTED "用户信息为空，无法查询权限等级——执行，拒绝。"

namespace alpha_permission {
	void setBotOwner(std::string id);
	void setUserData(jsonFileMap& user_data);
}

int checkIsOperator(CQuser& user);
int checkIsOperator(const CQuser& user);
//not finished
int checkIsBlacklisted(const CQuser& user);

int checkPermissionLevel(int user_permission_level, int required_permission_level, std::string& response_msg);
int checkPermissionLevel(const CQGroupMsg& group_msg, int required_permission_level);
int checkPermission(const CQmsg& msg, int required_permission_level);
int getGrpmbrPermissionLvl(CQGroupMember& group_member);
int getGrpmbrPermissionLvl(const CQGroupMember& group_member);

int setOperator(CQmsg& msg);
void register_setOperator(std::vector<CQMsgEvent>& event_list);

int delOperator(CQmsg& msg);
void register_delOperator(std::vector<CQMsgEvent>& event_list);

#endif // !ALPHA_PERMISSIONS_H
