#ifndef LOAD_FROM_JSON_H
#define LOAD_FROM_JSON_H

#include <string>
#include <json/json.h>

#define JSON_FIND_MATCH		0
#define JSON_FIND_CONTAIN	1

int loadStringByKeyword(std::string keyword,
						const Json::Value& json,
						std::string& str_dst);

int loadStringByKeyword(std::string keyword,
						Json::Value& json,
						std::string& str_dst,
						const char* default_str);

int loadIntByKeyword(std::string keyword,
					 const Json::Value& json,
					 int& int_dst);

int loadIntByKeyword(std::string keyword,
				  	 Json::Value& json,
					 int& int_dst,
					 int default_int);

int loadUInt64ByKeyword(std::string keyword,
	const Json::Value& json,
	uint64_t& uint64_dst);

//Return -1 if not find.
int getJsonIndexByKeyword(const std::string& keyword,
							const std::string& value,
							const Json::Value& json_array,
							int method = JSON_FIND_MATCH);

int setEmptyJson(Json::Value& _json, const std::string& _key, const Json::Value& _default);

Json::Value& getJsonByKeyword(Json::Value& json_arraylike, const std::string& keyword, int value);

Json::Value& getJsonByKeyword(Json::Value& json_arraylike, const std::string& keyword, uint64_t value);
const Json::Value& getJsonByKeyword(const Json::Value& json_arraylike, const std::string& keyword, uint64_t value);

#endif // !LOAD_FROM_JSON_H
