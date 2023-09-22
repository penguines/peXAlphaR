#include "cq_group.h"

CQgroup::CQgroup(){
	this->id = "";
	this->u64_id = 0;
}

CQgroup::CQgroup(Json::Value& group){
	this->load(group);
}

CQgroup::~CQgroup(){
	this->id.clear();
	this->group_data.clear();
}

void CQgroup::load(Json::Value& group){
	this->loadID(group);
}

void CQgroup::clear(){
	this->id = "";
	this->u64_id = 0;
	this->group_data.clear();
}

void CQgroup::loadID(Json::Value& group){
	loadUInt64ByKeyword("group_id", group, this->u64_id);
	loadStringByKeyword("group_id", group, this->id);
}