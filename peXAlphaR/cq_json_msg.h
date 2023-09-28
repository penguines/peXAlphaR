#ifndef CQ_JSON_MSG_H
#define CQ_JSON_MSG_H

#include <string>
#include <regex>
#include <json/json.h>
#include <opencv2/opencv.hpp>
#include "load_from_json.h"
#include "cq_transcoder.h"

#define STYLED_AT_REGEX			"\\[at=([0-9]+)\\]"
#define STYLEDTEXT_CQ_REGEX		"\\[CQ:([a-z]+),([a-z]+)=(\\S+?)\\]"

#define REPLACE_AT_SENDER		"$sender$"

class CQJsonMsg {
public:
	CQJsonMsg();
	CQJsonMsg(const Json::Value& json);
	CQJsonMsg(const std::string& text);
	CQJsonMsg(const std::string& type, const std::string& data_key, const std::string& data_value);
	virtual ~CQJsonMsg();

	CQJsonMsg& operator=(const std::string& text);
	CQJsonMsg& operator=(const char* text);
	CQJsonMsg& operator=(const CQJsonMsg& json_msg);
	CQJsonMsg& operator=(const Json::Value& json);

	CQJsonMsg& operator+(const std::string& text);
	CQJsonMsg& operator+(const char* text);
	CQJsonMsg& operator+(const CQJsonMsg& json_msg);
	CQJsonMsg& operator+(const Json::Value& json);

	CQJsonMsg& operator+=(const std::string& text);
	CQJsonMsg& operator+=(const char* text);
	CQJsonMsg& operator+=(const CQJsonMsg& json_msg);
	CQJsonMsg& operator+=(const Json::Value& json);

	CQJsonMsg& append(const std::string& text);
	CQJsonMsg& append(const char* text);
	CQJsonMsg& append(const CQJsonMsg& json_msg);
	CQJsonMsg& append(const Json::Value& json);

	void clear();
	void textToUTF8();
	const Json::Value& getJson() const;

	void toStyledText(std::string& text) const;
	void fromStyledText(const std::string& text);

	void replaceSpecialAt(const std::string& id, const std::string& special_key);

	void fillImgPath(const std::string& folder_path);
private:
	Json::Value m_json;
};

namespace CQJmsg {
	const Json::Value& text(const char* msg_text);
	const Json::Value& text(const std::string& msg_text);
	const Json::Value& at(const char* user_id);
	const Json::Value& at(const std::string& user_id);
	const Json::Value& image(const char* img_full_path);
	const Json::Value& image(const std::string& img_full_path);

	void styledTextEscape(std::string& text);
	void styledTextUnescape(std::string& text);

#ifdef OPENCV_ALL_HPP
	const Json::Value& image(cv::InputArray img, int png_quality = 1);
#endif

};

#endif