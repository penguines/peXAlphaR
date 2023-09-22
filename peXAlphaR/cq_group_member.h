#ifndef CQ_GROUP_MBR_H
#define CQ_GROUP_MBR_H

#include "cq_user.h"
#include "cq_group.h"

class CQGroupMember : public CQuser {
public:
	//群昵称
	std::string card;
	//头衔
	std::string title;
	//群主/管理员/成员
	std::string role;
	//地区
	std::string area;
	//等级
	int level;
	//权限加成
	int permission_addition;
	//所属群聊
	pCQgroup_t group;
	//自定义部分的群成员数据
	pJsonFile_t customed_grpmbr_data;

	CQGroupMember();
	CQGroupMember(Json::Value& user, pCQgroup_t grp = nullptr);
	~CQGroupMember();

	void load(Json::Value& user);
	void clear();
private:
	CQgroup_t m_group;

	void loadCard(Json::Value& user);
	void loadTitle(Json::Value& user);
	void loadRole(Json::Value& user);
	void loadArea(Json::Value& user);
	void loadLevel(Json::Value& user);
};

typedef CQGroupMember	CQGroupMember_t;
typedef CQGroupMember*	pCQGroupMember_t;

#endif // !CQ_GROUP_MBR_H
