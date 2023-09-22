#include "cq_group_member.h"

CQGroupMember::CQGroupMember() : CQuser(){
	this->card = "";
	this->title = "";
	this->role = "";
	this->area = "";
	this->level = 0;
	this->permission_addition = 0;
	this->group = &m_group;
	this->customed_grpmbr_data = nullptr;
}

CQGroupMember::CQGroupMember(Json::Value& user, pCQgroup_t grp){
	this->load(user);
	if (grp != nullptr) {
		this->group = grp;
	}
	else {
		this->group = &m_group;
	}
}

CQGroupMember::~CQGroupMember(){

}

void CQGroupMember::load(Json::Value& user){
	CQuser::load(user);
	loadCard(user);
	loadTitle(user);
	loadRole(user);
	loadArea(user);
	loadLevel(user);
}

void CQGroupMember::clear(){
	CQuser::clear();
	this->card = "";
	this->title = "";
	this->role = "";
	this->area = "";
	this->level = 0;
	this->permission_addition = 0;
	m_group.clear();
	this->group = &m_group;
	this->customed_grpmbr_data = nullptr;
}

void CQGroupMember::loadCard(Json::Value& user){
	loadStringByKeyword("card", user, this->card);
}

void CQGroupMember::loadTitle(Json::Value& user){
	loadStringByKeyword("title", user, this->title);
}

void CQGroupMember::loadRole(Json::Value& user){
	loadStringByKeyword("role", user, this->role);
	if (this->role == "member") {
		this->permission_addition = 0;
	}
	else if (this->role == "admin") {
		this->permission_addition = 1;
	}
	else if (this->role == "owner") {
		this->permission_addition = 2;
	}
}

void CQGroupMember::loadArea(Json::Value& user){
	loadStringByKeyword("area", user, this->area);
}

void CQGroupMember::loadLevel(Json::Value& user){
	if (loadIntByKeyword("level", user, this->level) == 0) {
		this->level = 0;
	}
}

