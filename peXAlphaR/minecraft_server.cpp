#include "minecraft_server.h"

uint64_t toVarInt(uint32_t val){
    uint64_t var_int = 0;
    while (1) {
        var_int <<= 8;
        uint8_t tmp = val & VARINT_SEGMENT_BITS;
        val >>= 7;
        var_int |= tmp;
        if (val) {
            var_int |= VARINT_CONTINUE_BIT;
        }
        else {
            break;
        }
    }
    return var_int;
}

uint8_t usedBytes(uint64_t val){
    if (val == 0) {
        return 1;
    }
    for (int8_t i = 8; i > 0; i--) {
        if (val & 0xFF00000000000000) {
            return i;
        }
        else {
            val <<= 8;
        }
    }
}

void writeDataInt64(uint8_t** data, uint64_t val, uint32_t* pos){
    for (int32_t i = usedBytes(val) - 1; i >= 0; i--) {
        *(*data + *pos) = (val >> (i << 3)) & 0xFF;
        *pos = *pos + 1;
    }
}

void writeDataStr(uint8_t** data, const std::string& str, uint32_t* pos){
    for (auto iter = str.cbegin(); iter != str.cend(); iter++) {
        *(*data + *pos) = *iter;
        *pos = *pos + 1;
    }
}

void getServerInfo(const std::string& ip, uint16_t port, std::string& info){
    uint64_t protocol_version = toVarInt(-1);
    uint8_t packet_id = 0;
    uint32_t ip_length = ip.length();
    uint32_t ip_len_vi = toVarInt(ip_length);
    uint8_t next_state = toVarInt(1);
    uint32_t data_length =  1   //packet_id
                            + usedBytes(protocol_version)
                            + usedBytes(ip_len_vi)
                            + ip_length
                            + 2     //port
                            + 1;    //nextState
    uint32_t data_len_vi = toVarInt(data_length);
    uint32_t request_length = data_length + usedBytes(data_len_vi);
    
    uint8_t* data = (uint8_t*)malloc(request_length);
    uint32_t pos = 0;
    writeDataInt64(&data, toVarInt(request_length), &pos);
    writeDataInt64(&data, packet_id, &pos);
    //writeDataInt64();
}
