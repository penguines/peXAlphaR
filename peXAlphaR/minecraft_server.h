#ifndef MINECRAFT_SERVER_H
#define MINECRAFT_SERVER_H

#include "alpha_types.h"
#include "cq_api_post.h"

uint64_t	toVarInt(uint32_t val);
uint8_t		usedBytes(uint64_t val);
void		writeDataInt64(uint8_t** data, uint64_t val, uint32_t* pos);
void		writeDataStr(uint8_t** data, const std::string& str, uint32_t* pos);

void getServerInfo(const std::string& ip, uint16_t port, std::string& info);

#endif // !MINECRAFT_SERVER_H

