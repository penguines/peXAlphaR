#ifndef CQ_USER_H
#define CQ_USER_H

#include <string>
#include <json/json.h>
#include "load_from_json.h"
#include "json_file.h"

class CQuser {
public:
	//账号
	std::string id;
	uint64_t	u64_id;
	//昵称
	std::string nickname;
	//性别
	std::string sex;
	//年龄
	int age;
	//自定义部分的用户数据
	pJsonFile_t customed_user_data;

	CQuser();
	CQuser(Json::Value& user);
	virtual ~CQuser();

	virtual void load(Json::Value& user);
	virtual void clear();
private:
	void loadID(Json::Value& user);
	void loadNickname(Json::Value& user);
	void loadSex(Json::Value& user);
	void loadAge(Json::Value& user);
};

typedef CQuser		CQuser_t;
typedef CQuser*		pCQuser_t;

#endif

