#include "cq_connection.h"

int createConnection(SOCKET& sock, const char* host, int port, int protocol){
	sock = socket(AF_INET, SOCK_STREAM, protocol);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(host);
	sockAddr.sin_port = htons(port);
	return bind(sock, (sockaddr*)&sockAddr, sizeof(sockaddr));
}

void recvJsonMessage(SOCKET& sock, std::string& recv_msg, size_t buffer_size){
	char* m_buffer = (char*)malloc(buffer_size * sizeof(char));
	if (m_buffer != nullptr) {
		memset(m_buffer, 0, buffer_size * sizeof(char));
		SOCKADDR_IN clntAddr;
		int nSize = sizeof(SOCKADDR);
		SOCKET clntSock = accept(sock, (SOCKADDR*)&clntAddr, &nSize);

		int rev = recv(clntSock, m_buffer, 32767, 0);
		int content_length, cl_loc, pack1_length;
		std::string str_content_length;
		pack1_length = strlen(m_buffer);
		recv_msg = m_buffer;
		if ((cl_loc = recv_msg.find("Content-Length")) != std::string::npos) {
			str_content_length = recv_msg.substr(cl_loc + 16, recv_msg.find("\n", cl_loc) - 16 - cl_loc);
			content_length = atoi(str_content_length.c_str());
			if (content_length > 4096 && pack1_length <= 4096) {
				recv_msg = recv_msg.substr(recv_msg.find_first_of('{'));
				rev = recv(clntSock, m_buffer, 32767, 0);
				recv_msg += m_buffer;
				strcpy(m_buffer, recv_msg.c_str());
			}
			else {
				recv_msg = recv_msg.substr(recv_msg.find_first_of('{'));
			}
		}

		std::string response;
		response = HTTP_200_OK_RES;
		send(clntSock, response.c_str(), response.length(), 0);

		closesocket(clntSock);
	}
	free(m_buffer);
}

void closeConnection(SOCKET& sock){
	closesocket(sock);
}
