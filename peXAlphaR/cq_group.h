#ifndef CQ_GROUP_H
#define CQ_GROUP_H

#include <vector>
#include <string>
#include <json/json.h>
#include "load_from_json.h"
#include "json_file.h"

class CQgroup {
public:
	std::string id;
	uint64_t	u64_id;
	jsonFile group_data;

	CQgroup();
	CQgroup(Json::Value& group);
	virtual ~CQgroup();

	virtual void load(Json::Value& group);
	virtual void clear();
private:
	void loadID(Json::Value& group);
};

typedef CQgroup					CQgroup_t;
typedef CQgroup*				pCQgroup_t;
typedef std::unordered_map<uint64_t, CQgroup> CQGroupList;

#endif
