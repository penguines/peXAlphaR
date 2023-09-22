#include "cq_api_post.h"

static std::string post_host;
static std::string gocq_root_folder;
static int post_port;

#ifndef ALPHA_POST_WITH_JSON

//0-> UTF8 ; 1->GB2312
void sendHttpData(std::string group_id, const char* msg, int mode = 0) {
	std::string d_data = encodeNL(msg);
	std::string m_data = "/send_group_msg?group_id=" + group_id + "&message=" + (mode ? d_data : UrlUTF8((char*)encodeNL(msg).c_str()));
	struct hostent* p_hostent = gethostbyname(post_host.c_str());
	if (p_hostent == NULL){
		return;
	}
	sockaddr_in addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = inet_addr(post_host.c_str());
	addr_server.sin_port = htons(post_port);
	memcpy(&(addr_server.sin_addr), p_hostent->h_addr_list[0], sizeof(addr_server.sin_addr));
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int res = connect(sock, (sockaddr*)&addr_server, sizeof(addr_server));
	if (res == -1){
#ifdef ALPHA_SHOW_ALL_INFO
		std::cout << "Connect failed " << std::endl;
#endif
		closesocket(sock);
		return;
	}
	std::string sendData = "GET " + m_data + " HTTP/1.1\r\n";
	sendData += "Host:" + post_host + "\r\n";
	sendData += "Connection:close\r\n";
	sendData += "\r\n";
	send(sock, sendData.c_str(), sendData.size(), 0);
	std::string  m_readBuffer;
	if (m_readBuffer.empty()) {
		m_readBuffer.resize(32767);
	}
	int readCount = recv(sock, &m_readBuffer[0], m_readBuffer.size(), 0);
#ifdef ALPHA_SHOW_ALL_INFO
	std::cout << "Request: " << sendData.c_str() << " and response:" << m_readBuffer.c_str() << std::endl;
#endif
	closesocket(sock);
}

void sendGroupMsg(std::string group_id, const char* msg, int isUTF8) {
	sendHttpData(group_id, msg, isUTF8);
}

void sendPrivateMsg(std::string id, const char* msg, int isUTF8) {
	if (!isUTF8) {
		sendCommand("/send_private_msg?user_id=" + id + "&message=" + UrlUTF8((char*)encodeNL(msg).c_str()));
	}
	else {
		sendCommand("/send_private_msg?user_id=" + id + "&message=" + encodeNL(msg));
	}
}

#else

void sendGroupMsg(std::string group_id, const char* msg, int isUTF8) {
	CQJsonMsg json_msg;
	if (!isUTF8) {
		json_msg.append(GB2312ToUTF8(msg));
	}
	else {
		json_msg.append(msg);
	}
	sendGroupMsg(group_id, json_msg.getJson());
}

void sendPrivateMsg(std::string id, const char* msg, int isUTF8) {
	CQJsonMsg json_msg;
	if (!isUTF8) {
		json_msg.append(GB2312ToUTF8(msg));
	}
	else {
		json_msg.append(msg);
	}
	sendPrivateMsg(id, json_msg.getJson());
}

void sendGroupMsg(const std::string& group_id, const Json::Value& json){
	Json::FastWriter fast_writer;
	std::string cmd("/send_group_msg?group_id=");
	cmd.append(group_id).append("&message=");
	cmd.append(urlEncode(fast_writer.write(json)));
	sendCommand(cmd);
}

void sendPrivateMsg(const std::string& user_id, const Json::Value& json) {
	Json::FastWriter fast_writer;
	std::string cmd("/send_private_msg?user_id=");
	cmd.append(user_id).append("&message=");
	cmd.append(urlEncode(fast_writer.write(json)));
	sendCommand(cmd);
}

void sendReply(const CQmsg& msg, const Json::Value& json){
	switch (msg.msg_type) {
	case MSG_GROUP:
		sendGroupMsg(((CQGroupMsg&)msg).group->id, json);
		break;
	case MSG_PRIVATE:
		sendPrivateMsg(((CQPrivateMsg&)msg).sender->id, json);
		break;
	}
}

#endif // !ALPHA_POST_WITH_JSON

std::string sendCommand(const std::string& command) {
	struct hostent* p_hostent = gethostbyname(post_host.c_str());
	if (p_hostent == NULL) {
		return "";
	}
	sockaddr_in addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = inet_addr(post_host.c_str());
	addr_server.sin_port = htons(post_port);
	memcpy(&(addr_server.sin_addr), p_hostent->h_addr_list[0], sizeof(addr_server.sin_addr));
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int res = connect(sock, (sockaddr*)&addr_server, sizeof(addr_server));
	if (res == -1) {
#ifdef ALPHA_SHOW_ALL_INFO
		std::cout << "Connect failed " << std::endl;
#endif
		closesocket(sock);
		return "";
	}

	std::string sendData;
	sendData.append("GET ").append(command).append(" HTTP/1.1\r\n");
	sendData += "Host:" + post_host + "\r\n";
	sendData += "Connection:close\r\n";
	sendData += "\r\n";
	send(sock, sendData.c_str(), sendData.size(), 0);
	std::string m_readBuffer;
	char* recvTmp = (char*)malloc(65537 * sizeof(char));
	int readCount = recv(sock, recvTmp, 65536, 0);
	recvTmp[readCount] = '\0';
	m_readBuffer = recvTmp;
	int content_length, cl_loc, cntTmp = readCount;
	std::string recvStr = m_readBuffer;
	if (m_readBuffer.length() >= 4096) {
		recvStr = recvStr.substr(recvStr.find_first_of('{'));
		while (cntTmp > 0) {
			cntTmp = recv(sock, recvTmp, 65536, 0);
			readCount += cntTmp;
			recvTmp[cntTmp] = '\0';
			m_readBuffer += recvTmp;
		}
	}
	free(recvTmp);
	closesocket(sock);
	return m_readBuffer;
}

void sendGroupMsg(std::string group_id, const std::string& msg, int isUTF8) {
	sendGroupMsg(group_id, msg.c_str(), isUTF8);
}

void sendPrivateMsg(std::string id, const std::string& msg, int isUTF8) {
	sendPrivateMsg(id, msg.c_str(), isUTF8);
}

void sendReply(const CQmsg& msg, const std::string& reply, int isUTF8) {
	sendReply(msg, reply.c_str(), isUTF8);
}

void sendReply(const CQmsg& msg, const char* reply, int isUTF8) {
	switch (msg.msg_type) {
	case MSG_GROUP:
		sendGroupMsg(((const CQGroupMsg&)msg).group->id, reply, isUTF8);
		break;
	case MSG_PRIVATE:
		sendPrivateMsg(((const CQPrivateMsg&)msg).sender->id, reply, isUTF8);
		break;
	}
}


void getImage(const std::string& file, std::string& file_path, size_t& size) {
	std::string tmp;
	std::string cmd("/get_image?file=");
	Json::Reader rd;
	Json::Value response;
	cmd.append(file);
	tmp = sendCommand(cmd);
	tmp = tmp.substr(tmp.find_first_of('{'));
	if (rd.parse(tmp, response)) {
		if (response.isMember("data")) {
			Json::Value& data_tmp = response["data"];
			loadStringByKeyword("file", data_tmp, tmp);
			loadUInt64ByKeyword("size", data_tmp, size);
			file_path.assign(gocq_root_folder);
			file_path.append(tmp);
		}
		else {
			file_path.assign("");
		}
	}
	else {
		file_path.assign("");
	}
}


void cq_post_api::setPostIP(std::string host, int port){
	post_host = host;
	post_port = port;
}

void cq_post_api::setGoCQFolder(const std::string& folder){
	gocq_root_folder = folder;
}
