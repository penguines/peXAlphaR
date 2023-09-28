#include "cq_json_msg.h"

CQJsonMsg::CQJsonMsg(){
	this->m_json.clear();
}

CQJsonMsg::CQJsonMsg(const Json::Value& json){
	this->append(json);
}

CQJsonMsg::CQJsonMsg(const std::string& text){
	this->append(text);
}

CQJsonMsg::CQJsonMsg(const std::string& type, const std::string& data_key, const std::string& data_value){
	static thread_local Json::Value json_msg_tmp;
	static thread_local bool is_init = false;
	if (is_init == false) {
		json_msg_tmp["type"] = type;
		json_msg_tmp["data"][data_key] = "";
		is_init = true;
	}
	json_msg_tmp["data"][data_key] = data_value;
	this->m_json.append(json_msg_tmp);
}

CQJsonMsg::~CQJsonMsg(){

}

CQJsonMsg& CQJsonMsg::operator=(const std::string& text){
	this->m_json.clear();
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator=(const char* text){
	this->m_json.clear();
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator=(const CQJsonMsg& json_msg){
	if (this == &json_msg) {
		return *this;
	}
	this->m_json.clear();
	this->m_json.copy(json_msg.getJson());
	return *this;
}

CQJsonMsg& CQJsonMsg::operator=(const Json::Value& json){
	this->m_json.clear();
	this->m_json.append(json);
	return *this;
}

CQJsonMsg& CQJsonMsg::operator+(const std::string& text){
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator+(const char* text){
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator+(const CQJsonMsg& json_msg){
	return this->append(json_msg);
}

CQJsonMsg& CQJsonMsg::operator+(const Json::Value& json){
	return this->append(json);
}

CQJsonMsg& CQJsonMsg::operator+=(const std::string& text){
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator+=(const char* text){
	return this->append(text);
}

CQJsonMsg& CQJsonMsg::operator+=(const CQJsonMsg& json_msg){
	return this->append(json_msg);
}

CQJsonMsg& CQJsonMsg::operator+=(const Json::Value& json){
	return this->append(json);
}

CQJsonMsg& CQJsonMsg::append(const std::string& text){
	return this->append(text.c_str());
}

CQJsonMsg& CQJsonMsg::append(const char* text){
	this->m_json.append(CQJmsg::text(text));
	return *this;
}

CQJsonMsg& CQJsonMsg::append(const CQJsonMsg& json_msg){
	const Json::Value& json_tmp = json_msg.getJson();
	int sz = 0;
	if (!json_tmp.empty()) {
		sz = json_tmp.size();
		for (int i = 0; i < sz; i++) {
			this->m_json.append(json_tmp[i]);
		}
	}
	return *this;
}

CQJsonMsg& CQJsonMsg::append(const Json::Value& json){
	if (!json.empty()) {
		this->m_json.append(json);
	}
	return *this;
}

void CQJsonMsg::clear(){
	this->m_json.clear();
}

void CQJsonMsg::textToUTF8(){
	if (this->m_json.empty()) {
		return;
	}
	std::string text_tmp;
	for (int i = 0; i < this->m_json.size(); i++) {
		loadStringByKeyword("type", m_json[i], text_tmp);
		if (text_tmp == "text") {
			Json::Value& json_tmp = m_json[i]["data"];
			loadStringByKeyword("text", json_tmp, text_tmp);
			text_tmp = GB2312ToUTF8(text_tmp);
			m_json[i]["data"]["text"] = text_tmp;
		}
	}
}

const Json::Value& CQJsonMsg::getJson() const{
	return static_cast<const Json::Value&>(this->m_json);
}

void CQJsonMsg::toStyledText(std::string& text) const{
	if (this->m_json.empty()) {
		return;
	}
	std::string text_tmp;
	for (int i = 0; i < this->m_json.size(); i++) {
		loadStringByKeyword("type", m_json[i], text_tmp);
		const Json::Value& json_tmp = m_json[i]["data"];
		if (text_tmp == "text") {
			loadStringByKeyword("text", json_tmp, text_tmp);
			CQJmsg::styledTextEscape(text_tmp);
			text.append(text_tmp);
		}
		else if(text_tmp == "at") {
			loadStringByKeyword("qq", json_tmp, text_tmp);
			text.append("[CQ:at,qq=").append(text_tmp).append("]");
		}
		else if (text_tmp == "image") {
			loadStringByKeyword("file", json_tmp, text_tmp);
			text.append("[CQ:image,file=").append(text_tmp).append("]");
		}
	}
}

void CQJsonMsg::fromStyledText(const std::string& text){
	std::regex reg(STYLEDTEXT_CQ_REGEX);
	std::smatch reg_result;
	
	this->clear();
	auto text_iter = text.cbegin();
	while (std::regex_search(text_iter, text.cend(), reg_result, reg)) {
		//text before CQcodes
		std::string msg_text(text_iter, reg_result[0].first);
		CQJmsg::styledTextUnescape(msg_text);
		this->append(msg_text);

		//CQ codes
		std::string cq_type(reg_result[1].first, reg_result[1].second);
		if (cq_type == "at") {
			this->append(CQJmsg::at(reg_result[3].str()));
		}
		else if (cq_type == "image") {
			this->append(CQJmsg::image(reg_result[3].str()));
		}

		text_iter = reg_result[0].second;
	}
	//text after CQcodes
	if (text_iter != text.cend()) {
		std::string msg_text(text_iter, text.cend());
		CQJmsg::styledTextUnescape(msg_text);
		this->append(msg_text);
	}

}

void CQJsonMsg::replaceSpecialAt(const std::string& id, const std::string& special_key) {
	if (this->m_json.empty()) {
		return;
	}
	std::string text_tmp;
	for (int i = 0; i < this->m_json.size(); i++) {
		loadStringByKeyword("type", this->m_json[i], text_tmp);
		if (text_tmp == "at") {
			Json::Value& json_tmp = this->m_json[i]["data"];
			loadStringByKeyword("qq", json_tmp, text_tmp);
			if (text_tmp == special_key) {
				json_tmp["qq"] = id;
			}
		}
	}
}

void CQJsonMsg::fillImgPath(const std::string& folder_path){
	if (this->m_json.empty()) {
		return;
	}
	std::string text_tmp, fp_tmp("file:///");
	fp_tmp.append(folder_path);
	for (int i = 0; i < this->m_json.size(); i++) {
		loadStringByKeyword("type", this->m_json[i], text_tmp);
		if (text_tmp == "image") {
			Json::Value& json_tmp = this->m_json[i]["data"];
			loadStringByKeyword("file", json_tmp, text_tmp);
			if (text_tmp.find("/") == std::string::npos && text_tmp.find("\\") == std::string::npos) {
				json_tmp["file"] = fp_tmp + text_tmp;
			}
		}
	}
}

const Json::Value& CQJmsg::text(const char* msg_text){
	static thread_local Json::Value json_text_tmp;
	static thread_local bool is_init = false;
	if (is_init == false) {
		json_text_tmp["type"] = "text";
		json_text_tmp["data"]["text"] = "";
		is_init = true;
	}
	json_text_tmp["data"]["text"] = msg_text;
	return json_text_tmp;
}

const Json::Value& CQJmsg::text(const std::string& msg_text) {
	return CQJmsg::text(msg_text.c_str());
}

const Json::Value& CQJmsg::at(const char* user_id){
	static thread_local Json::Value json_at_tmp;
	static thread_local bool is_init = false;
	if (is_init == false) {
		json_at_tmp["type"] = "at";
		json_at_tmp["data"]["qq"] = "";
		is_init = true;
	}
	json_at_tmp["data"]["qq"] = user_id;
	return json_at_tmp;
}

const Json::Value& CQJmsg::at(const std::string& user_id){
	return CQJmsg::at(user_id.c_str());
}

const Json::Value& CQJmsg::image(const char* img_full_path){
	static thread_local Json::Value json_img_tmp;
	static thread_local bool is_init = false;
	if (is_init == false) {
		json_img_tmp["type"] = "image";
		json_img_tmp["data"]["file"] = "";
		is_init = true;
	}
	json_img_tmp["data"]["file"] = img_full_path;
	return json_img_tmp;
}

const Json::Value& CQJmsg::image(const std::string& img_full_path){
	return CQJmsg::image(img_full_path.c_str());
}

void CQJmsg::styledTextEscape(std::string& text){
	int i = 0;
	while ((i = text.find("%", i)) != std::string::npos) {
		text.replace(i, 1, "%25");
		i += 2;
	}
	i = 0;
	while ((i = text.find("[", i)) != std::string::npos) {
		text.replace(i, 1, "%5B");
		i += 2;
	}
	i = 0;
	while ((i = text.find("]", i)) != std::string::npos) {
		text.replace(i, 1, "%5D");
		i += 2;
	}
	i = 0;
	while ((i = text.find("=", i)) != std::string::npos) {
		text.replace(i, 1, "%3D");
		i += 2;
	}
	i = 0;
	while ((i = text.find("$", i)) != std::string::npos) {
		text.replace(i, 1, "%24");
		i += 2;
	}
}

void CQJmsg::styledTextUnescape(std::string& text){
	int i = 0;
	while ((i = text.find("%5B", i)) != std::string::npos) {
		text.replace(i, 3, "[");
		i++;
	}
	i = 0;
	while ((i = text.find("%5D", i)) != std::string::npos) {
		text.replace(i, 3, "]");
		i++;
	}
	i = 0;
	while ((i = text.find("%3D", i)) != std::string::npos) {
		text.replace(i, 3, "=");
		i++;
	}
	i = 0;
	while ((i = text.find("%24", i)) != std::string::npos) {
		text.replace(i, 3, "$");
		i++;
	}
	i = 0;
	while ((i = text.find("%25", i)) != std::string::npos) {
		text.replace(i, 3, "%");
		i++;
	}
}

#ifdef OPENCV_ALL_HPP

const Json::Value& CQJmsg::image(cv::InputArray img, int png_quality){
	std::vector<uchar> img_base64;
	std::vector<int> png_param(2);
	png_param[0] = cv::IMWRITE_PNG_COMPRESSION;
	png_param[1] = png_quality;
	cv::imencode(".png", img, img_base64, png_param);
	std::string base64_str;
	base64Encode(img_base64, base64_str);
	return CQJmsg::image("base64://" + base64_str);
}

#endif