#include "cq_user.h"

CQuser::CQuser(){
	this->id = "";
	this->u64_id = 0;
	this->nickname = "";
	this->sex = "unknown";
	this->age = -1;
	this->customed_user_data = nullptr;
}

CQuser::CQuser(Json::Value& user) {
	this->load(user);
}

CQuser::~CQuser(){
	this->id.clear();
	this->nickname.clear();
	this->sex.clear();
}

void CQuser::load(Json::Value& user){
	this->loadID(user);
	this->loadNickname(user);
	this->loadSex(user);
	this->loadAge(user);
}

void CQuser::clear(){
	this->id = "";
	this->u64_id = 0;
	this->nickname = "";
	this->sex = "unknown";
	this->age = -1;
	this->customed_user_data = nullptr;
}

void CQuser::loadID(Json::Value& user){
	loadUInt64ByKeyword("user_id", user, this->u64_id);
	loadStringByKeyword("user_id", user, this->id);
}

void CQuser::loadNickname(Json::Value& user) {
	loadStringByKeyword("nickname", user, this->nickname);
}

void CQuser::loadSex(Json::Value& user) {
	loadStringByKeyword("sex", user, this->sex);
}

void CQuser::loadAge(Json::Value& user) {
	if (loadIntByKeyword("age", user, this->age) == 0) {
		this->age = -1;
	}
}