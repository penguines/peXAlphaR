#ifndef CQ_MSG_H
#define CQ_MSG_H

#include <string>
#include <vector>
#include <json/json.h>
#include "cq_transcoder.h"
#include "cq_user.h"
#include "cq_group.h"
#include "cq_group_member.h"

typedef enum messageType{ MSG_UNDEFINED = -1, MSG_PRIVATE = 0, MSG_GROUP = 1 };

class CQmsg {
public:
	//数组格式消息
	Json::Value content;
	//原始字符串格式消息
	std::string raw_message;
	std::string msg_id;
	std::string self_id;
	std::string sub_type;
	//-1: Undefined, 0: Private Message, 1: Group Message;
	int msg_type;
	unsigned long long time;

	CQmsg();
	CQmsg(Json::Value& message);
	virtual ~CQmsg();

	virtual void load(Json::Value& message);
	virtual void clear();

	void updateText();
	//return text as UTF-8
	const std::string& text() const;
	const std::string& toStyledText() const;

#ifdef CQ_TRANSCODER_H
	
	//return text as unicode
	const std::string& uniText() const;

#endif // CQ_TRANSCODER_H


private:
	void loadContent(Json::Value& message);
	void loadRawMsg(Json::Value& message);
	void loadMsgID(Json::Value& message);
	void loadSelfID(Json::Value& message);
	void loadSubType(Json::Value& message);
	void loadMsgType(Json::Value& message);
	void loadTime(Json::Value& message);
	std::string m_text;
	std::string m_styled_text;
	std::string m_unicode_text;
};

class CQPrivateMsg : public CQmsg {
public:
	pCQuser_t sender;
	pCQuser_t target;

	CQPrivateMsg();
	CQPrivateMsg(Json::Value& message);
	~CQPrivateMsg();

	//将会重新加载m_sender和m_target并将指针重新指向内部m_sender和m_target
	void load(Json::Value& message);
	void clear();
private:
	CQuser_t m_sender;
	CQuser_t m_target;

	void loadSender(Json::Value& message);
	void loadTarget(Json::Value& message);
};


class CQGroupMsg : public CQmsg {
public:
	pCQGroupMember_t	sender;
	pCQgroup_t			group;
	std::string			msg_seq;

	CQGroupMsg();
	CQGroupMsg(Json::Value& message);
	~CQGroupMsg();

	//将会重新加载m_sender和m_group并将指针重新指向内部m_sender和m_group
	void load(Json::Value& message);
	void clear();
private:
	CQGroupMember_t m_sender;
	CQgroup_t		m_group;

	void loadSender(Json::Value& message);
	void loadGroup(Json::Value& message);
	void loadMsgSeq(Json::Value& message);
};

pCQuser_t	getMsgSender(const CQmsg& msg);
std::string getMsgSenderID(const CQmsg& msg);
std::string getMsgReplyID(const CQmsg& msg);

#endif // !CQ_MSG_H
