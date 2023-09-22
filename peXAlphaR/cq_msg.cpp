#include "cq_msg.h"

CQmsg::CQmsg(){
	this->raw_message = "";
	this->msg_id = "";
	this->self_id = "";
	this->sub_type = "";
	this->msg_type = messageType::MSG_UNDEFINED;
	this->time = 0;
}

CQmsg::CQmsg(Json::Value& message){
	this->load(message);
}

CQmsg::~CQmsg(){
	
}

void CQmsg::load(Json::Value& message){
	loadContent(message);
	loadRawMsg(message);
	loadMsgID(message);
	loadSelfID(message);
	loadSubType(message);
	loadMsgType(message);
	loadTime(message);
	updateText();
}

void CQmsg::clear(){
	this->raw_message.clear();
	this->msg_id.clear();
	this->self_id.clear();
	this->sub_type.clear();
	this->msg_type = -1;
	this->time = 0;
}

void CQmsg::updateText(){
	this->m_text.clear();
	this->m_styled_text.clear();
	std::string str_temp;
	if (this->content.isArray()) {
		for (int i = 0; i < this->content.size(); i++) {
			loadStringByKeyword("type", this->content[i], str_temp);
			if (str_temp == "text") {
				if (content[i].isMember("data")) {
					loadStringByKeyword("text", content[i]["data"], str_temp);
					this->m_styled_text.append(str_temp);
					this->m_text.append(str_temp);
				}
			}
			else if (str_temp == "image") {
				if (content[i].isMember("data")) {
					loadStringByKeyword("file", content[i]["data"], str_temp);
					this->m_styled_text.append("[img=").append(str_temp).append("]");
				}
			}
			else if (str_temp == "at") {
				if (content[i].isMember("data")) {
					loadStringByKeyword("qq", content[i]["data"], str_temp);
					this->m_styled_text.append("[at=").append(str_temp).append("]");
				}
			}
		}
	}
	this->m_unicode_text = utf8ToGB2312(this->m_text);
}

const std::string& CQmsg::text() const{
	return this->m_text;
}

const std::string& CQmsg::toStyledText() const{
	return this->m_styled_text;
}

#ifdef CQ_TRANSCODER_H

const std::string& CQmsg::uniText() const {
	return this->m_unicode_text;
}

#endif // CQ_TRANSCODER_H

void CQmsg::loadContent(Json::Value& message){
	if (message.isMember("message")) {
		this->content.copy(static_cast<const Json::Value&>(message["message"]));
	}
}

void CQmsg::loadRawMsg(Json::Value& message){
	loadStringByKeyword("raw_message", message, this->raw_message);
}

void CQmsg::loadMsgID(Json::Value& message){
	loadStringByKeyword("message_id", message, this->msg_id);
}

void CQmsg::loadSelfID(Json::Value& message){
	loadStringByKeyword("self_id", message, this->self_id);
}

void CQmsg::loadSubType(Json::Value& message){
	loadStringByKeyword("sub_type", message, this->sub_type);
}

void CQmsg::loadMsgType(Json::Value& message){
	std::string type_tmp;
	loadStringByKeyword("message_type", message, type_tmp);
	if (type_tmp == "private") {
		this->msg_type = messageType::MSG_PRIVATE;
	}
	else if (type_tmp == "group") {
		this->msg_type = messageType::MSG_GROUP;
	}
	else {
		this->msg_type = messageType::MSG_UNDEFINED;
	}
}

void CQmsg::loadTime(Json::Value& message){
	if (message.isMember("time")) {
		this->time = message["time"].asInt64();
	}
	else {
		this->time = 0;
	}
}

//-------------------------------------------Private Message-------------------------------------------//

CQPrivateMsg::CQPrivateMsg() : CQmsg(){
	this->sender = &m_sender;
	this->target = &m_target;
}

CQPrivateMsg::CQPrivateMsg(Json::Value& message){
	this->load(message);
}

CQPrivateMsg::~CQPrivateMsg(){
}

void CQPrivateMsg::load(Json::Value& message){
	CQmsg::load(message);
	this->loadSender(message);
	this->loadTarget(message);
}

void CQPrivateMsg::clear(){
	CQmsg::clear();
	this->m_sender.clear();
	this->m_target.clear();
	this->sender = &m_sender;
	this->target = &m_target;
}

void CQPrivateMsg::loadSender(Json::Value& message){
	this->m_sender.clear();
	if (message.isMember("sender")) {
		Json::Value& tmp = message["sender"];
		this->m_sender.load(tmp);
	}
	this->sender = &m_sender;
}

void CQPrivateMsg::loadTarget(Json::Value& message) {
	this->m_target.clear();
	loadStringByKeyword("target_id", message, this->m_target.id);
	this->target = &m_target;
}

//-------------------------------------------Group Message-------------------------------------------//

CQGroupMsg::CQGroupMsg() : CQmsg(){
	this->msg_seq = "";
	this->sender = &m_sender;
	this->group = &m_group;
}

CQGroupMsg::CQGroupMsg(Json::Value& message){
	this->load(message);
}

CQGroupMsg::~CQGroupMsg(){
}

void CQGroupMsg::load(Json::Value& message){
	CQmsg::load(message);
	this->loadSender(message);
	this->loadGroup(message);
	this->loadMsgSeq(message);
}

void CQGroupMsg::clear(){
	CQmsg::clear();
	this->m_sender.clear();
	this->m_group.clear();
	this->msg_seq.clear();
	this->sender = &m_sender;
	this->group = &m_group;
}

void CQGroupMsg::loadSender(Json::Value& message){
	this->m_sender.clear();
	if (message.isMember("sender")) {
		Json::Value& tmp = message["sender"];
		this->m_sender.load(tmp);
	}
	this->sender = &m_sender;
}

void CQGroupMsg::loadGroup(Json::Value& message) {
	this->m_group.clear();
	this->m_group.load(message);
	this->group = &m_group;
}

void CQGroupMsg::loadMsgSeq(Json::Value& message){
	loadStringByKeyword("message_seq", message, this->msg_seq);
}

pCQuser_t getMsgSender(const CQmsg& msg){
	switch (msg.msg_type) {
	case MSG_PRIVATE:
		return ((const CQPrivateMsg&)msg).sender;
	case MSG_GROUP:
		return ((const CQGroupMsg&)msg).sender;
	}
	return nullptr;
}

std::string getMsgSenderID(const CQmsg& msg){
	switch (msg.msg_type) {
	case MSG_PRIVATE:
		return ((const CQPrivateMsg&)msg).sender->id;
	case MSG_GROUP:
		return ((const CQGroupMsg&)msg).sender->id;
	}
	return "";
}

std::string getMsgReplyID(const CQmsg& msg){
	switch (msg.msg_type) {
	case MSG_PRIVATE:
		return ((const CQPrivateMsg&)msg).sender->id;
	case MSG_GROUP:
		return ((const CQGroupMsg&)msg).group->id;
	}
	return "";
}
