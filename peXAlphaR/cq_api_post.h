#ifndef CQ_API_POST_H
#define CQ_API_POST_H

#include "alpha_debug.h"

#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <windows.h>
#include "cq_transcoder.h"
#include "cq_msg.h"

#define ALPHA_POST_WITH_JSON
#define ALPHA_SHOW_ALL_INFO

namespace cq_post_api {
	void setPostIP(std::string host = "127.0.0.1", int port = 5700);
	void setGoCQFolder(const std::string& folder);
}

#ifndef ALPHA_POST_WITH_JSON
//previous API
void sendHttpData(std::string, char*, int);

#else

#include "cq_json_msg.h"

void sendGroupMsg(const std::string& group_id, const Json::Value& json);
void sendPrivateMsg(const std::string& user_id, const Json::Value& json);
void sendReply(const CQmsg& msg, const Json::Value& json);

#endif

//latest API
std::string sendCommand(const std::string& command);

void sendGroupMsg(std::string group_id, const char* msg, int isUTF8 = 0);
void sendPrivateMsg(std::string id, const char* msg, int isUTF8 = 0);
void sendReply(const CQmsg& msg, const char* reply, int isUTF8);

void sendGroupMsg(std::string group_id, const std::string& msg, int isUTF8 = 0);
void sendPrivateMsg(std::string id, const std::string& msg, int isUTF8 = 0);
void sendReply(const CQmsg& msg, const std::string& reply, int isUTF8);

void getImage(const std::string& file, std::string& file_path, size_t& size);

#endif