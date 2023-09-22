#ifndef CQ_CONNECTION_H
#define CQ_CONNECTION_H

#include <string>
#include <WinSock2.h>

#define HTTP_200_OK_RES "HTTP/1.1 200 OK\r\nhead:123\r\n\r\nbody"

void createConnection(SOCKET& sock, const char* host, int port, int protocol = IPPROTO_TCP);
void recvJsonMessage(SOCKET& sock, std::string& recv_msg, size_t buffer_size = 32767);
void closeConnection(SOCKET& sock);

#endif // !CQ_CONNECTION_H
