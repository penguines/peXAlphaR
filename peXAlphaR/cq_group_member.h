#ifndef CQ_GROUP_MBR_H
#define CQ_GROUP_MBR_H

#include "cq_user.h"
#include "cq_group.h"

class CQGroupMember : public CQuser {
public:
	//Ⱥ�ǳ�
	std::string card;
	//ͷ��
	std::string title;
	//Ⱥ��/����Ա/��Ա
	std::string role;
	//����
	std::string area;
	//�ȼ�
	int level;
	//Ȩ�޼ӳ�
	int permission_addition;
	//����Ⱥ��
	pCQgroup_t group;
	//�Զ��岿�ֵ�Ⱥ��Ա����
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
