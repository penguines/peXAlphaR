#ifndef MINECRAFT_SERVER_H
#define MINECRAFT_SERVER_H

#include "alpha_types.h"
#include "cq_connection.h"
#include <string>
#include <WS2tcpip.h>

class ProtocolBuffer {
public:
	ProtocolBuffer();
	ProtocolBuffer(uint32_t _size);

	~ProtocolBuffer();

	uint32_t size() const;
	const uint8_t* data() const;
	
	uint8_t resize(uint32_t _size);
	void clear();

	ProtocolBuffer& append(uint8_t val);
	ProtocolBuffer& append(const varInt& val);
	ProtocolBuffer& append(const std::string& str);

private:
	uint32_t sz;
	uint32_t pos;
	uint8_t* buffer;
};

SOCKET& operator>>(SOCKET& sck, varInt& dst);

int getServerInfo(const std::string& ip, uint16_t port, std::string& info);

#endif // !MINECRAFT_SERVER_H

