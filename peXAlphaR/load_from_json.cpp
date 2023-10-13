#include "load_from_json.h"

int loadStringByKeyword(std::string keyword, const Json::Value& json, std::string& str_dst) {
	if (json.isNull()) {
		return 0;
	}
	if (json.isMember(keyword)) {
		str_dst = json[keyword].asString();
		return 1;
	}
	else {
		str_dst.clear();
		return 0;
	}
}

int loadStringByKeyword(std::string keyword, Json::Value& json, std::string& str_dst, const char* default_str) {
	if (!loadStringByKeyword(keyword, json, str_dst)) {
		json[keyword] = default_str;
		str_dst = default_str;
		return 1;
	}
	return 1;
}

int loadIntByKeyword(std::string keyword, const Json::Value& json, int& int_dst) {
	if (json.isNull()) {
		return 0;
	}
	if (json.isMember(keyword) && json[keyword].isInt()) {
		int_dst = json[keyword].asInt();
		return 1;
	}
	else {
		return 0;
	}
}

int loadIntByKeyword(std::string keyword, Json::Value& json, int& int_dst, int default_int) {
	if (!loadIntByKeyword(keyword, json, int_dst)) {
		json[keyword] = default_int;
		int_dst = default_int;
		return 1;
	}
	return 1;
}

int loadUInt64ByKeyword(std::string keyword, const Json::Value& json, uint64_t& uint64_dst){
	if (json.isNull()) {
		return 0;
	}
	if (json.isMember(keyword) && json[keyword].isUInt64()) {
		uint64_dst = json[keyword].asUInt64();
		return 1;
	}
	else {
		return 0;
	}
}

int getJsonIndexByKeyword(const std::string& keyword, const std::string& value, const Json::Value& json_array, int method){
	if (method == JSON_FIND_MATCH) {
		for (int i = 0; i < json_array.size(); i++) {
			if (json_array[i][keyword].asString() == value) {
				return i;
			}
		}
	}
	else {
		for (int i = 0; i < json_array.size(); i++) {
			if (json_array[i][keyword].asString().find(value) != std::string::npos) {
				return i;
			}
		}
	}
	return -1;
}

int setEmptyJson(Json::Value& _json, const std::string& _key, const Json::Value& _default){
	if (!_json.isMember(_key)) {
		_json[_key] = _default;
		return 1;
	}
	return 0;
}

Json::Value& getJsonByKeyword(Json::Value& json_arraylike, const std::string& keyword, int value){
	if (json_arraylike.isArray()) {
		for (auto iter = json_arraylike.begin(); iter != json_arraylike.end(); iter++) {
			if ((*iter)[keyword].asInt() == value) {
				return *iter;
			}
		}
	}
	return const_cast<Json::Value&>(Json::Value::nullSingleton());
}

Json::Value& getJsonByKeyword(Json::Value& json_arraylike, const std::string& keyword, uint64_t value){
	if (json_arraylike.isArray()) {
		for (auto iter = json_arraylike.begin(); iter != json_arraylike.end(); iter++) {
			if ((*iter)[keyword].asUInt64() == value) {
				return *iter;
			}
		}
	}
	return const_cast<Json::Value&>(Json::Value::nullSingleton());
}

const Json::Value& getJsonByKeyword(const Json::Value& json_arraylike, const std::string& keyword, uint64_t value) {
	if (json_arraylike.isArray()) {
		for (auto iter = json_arraylike.begin(); iter != json_arraylike.end(); iter++) {
			if ((*iter)[keyword].asUInt64() == value) {
				return *iter;
			}
		}
	}
	return Json::Value::nullSingleton();
}

Json::Value& getJsonByKeyword(Json::Value& json_arraylike, const std::string& keyword, const std::string& value){
	if (json_arraylike.isArray()) {
		for (auto iter = json_arraylike.begin(); iter != json_arraylike.end(); iter++) {
			if ((*iter)[keyword].asString() == value) {
				return *iter;
			}
		}
	}
	return const_cast<Json::Value&>(Json::Value::nullSingleton());
}

const Json::Value& getJsonByKeyword(const Json::Value& json_arraylike, const std::string& keyword, const std::string& value) {
	if (json_arraylike.isArray()) {
		for (auto iter = json_arraylike.begin(); iter != json_arraylike.end(); iter++) {
			if ((*iter)[keyword].asString() == value) {
				return *iter;
			}
		}
	}
	return Json::Value::nullSingleton();
}