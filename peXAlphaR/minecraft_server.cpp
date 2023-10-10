#include "minecraft_server.h"

ProtocolBuffer::ProtocolBuffer(){
    this->sz = 0;
    this->pos = 0;
    this->buffer = nullptr;
}

ProtocolBuffer::ProtocolBuffer(uint32_t _size){
    this->sz = _size;
    this->pos = 0;
    this->buffer = (uint8_t*)malloc(_size);
    if (this->buffer != nullptr) {
        memset(this->buffer, 0, _size);
    }
}

ProtocolBuffer::~ProtocolBuffer(){
    if (this->buffer != nullptr) {
        free(this->buffer);
    }
}

uint32_t ProtocolBuffer::size() const{
    return this->sz;
}

const uint8_t* ProtocolBuffer::data() const{
    return this->buffer;
}

uint8_t ProtocolBuffer::resize(uint32_t _size){
    if (_size != this->sz) {
        uint8_t* ptr = (uint8_t*)realloc(this->buffer, _size);
        if (ptr != nullptr) {
            this->buffer = ptr;
            this->sz = _size;
            return 1;
        }
        else {
            return 0;
        }
    }
    return 1;
}

void ProtocolBuffer::clear(){
    if (this->buffer != nullptr) {
        memset(this->buffer, 0, this->sz);
        this->pos = 0;
    }
}

ProtocolBuffer& ProtocolBuffer::append(uint8_t val){
    if (this->pos >= this->sz) {
        this->resize(this->pos + 1);
    }
    this->buffer[this->pos] = val;
    this->pos++;
    return *this;
}

ProtocolBuffer& ProtocolBuffer::append(const varInt& val){
    uint8_t sz_val = val.size();
    if (sz_val == 0) {
        return *this;
    }
    if (this->pos + sz_val - 1 >= this->sz) {
        this->resize(this->pos + sz_val);
    }
    const uint8_t* ptr_val = val.value();
    for (uint8_t i = 0; i < sz_val; i++) {
        *(this->buffer + this->pos) = *(ptr_val + i);
        this->pos++;
    }
    return *this;
}

ProtocolBuffer& ProtocolBuffer::append(const std::string& str){
    uint32_t len = str.length();
    if (len == 0) {
        return *this;
    }
    if (this->pos + len - 1 >= this->sz) {
        this->resize(this->pos + len);
    }
    for (auto iter = str.cbegin(); iter != str.cend(); iter++) {
        *(this->buffer + this->pos) = *iter;
        this->pos++;
    }
    return *this;
}

SOCKET& operator>>(SOCKET& sck, varInt& dst){
    uint8_t tmp = VARINT_CONTINUE_BIT;
    uint8_t recv_ints[5] = { 0 };
    uint8_t i = 0;
    while (tmp & VARINT_CONTINUE_BIT) {
        recv(sck, (char*)&tmp, 1, 0);
        recv_ints[i] = tmp;
        i++;
    }
    dst.assign(recv_ints, i);
    return sck;
}

int connectServer(SOCKET& sock, const std::string& ip, uint16_t port, int protocol){
    sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, protocol)) < 0) {
        return SOCKET_CREATE_ERROR;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &(serv_addr.sin_addr)) <= 0) {
        hostent* hptr = nullptr;
        hptr = gethostbyname(ip.c_str());
        if (hptr == nullptr) {
            return SOCKET_ADDRESS_ERROR;
        }
        serv_addr.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(in_addr*)hptr->h_addr_list[0]));
    }
    int client;
    if ((client = connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        return SOCKET_CONNECT_ERROR;
    }
    return 0;
}

int getServerInfo(const std::string& ip, uint16_t port, std::string& info){

    varInt protocol_version(-1);
    uint8_t packet_id = 0;

    uint32_t ip_length = ip.length();
    varInt ip_len_vi(ip_length);

    varInt next_state(1);
    uint32_t data_length =  1   //packet_id
                            + protocol_version.size()
                            + ip_len_vi.size()
                            + ip_length
                            + 2     //port
                            + 1;    //nextState
    varInt data_len_vi(data_length);
    uint32_t request_length = data_length + data_len_vi.size();
    
    ProtocolBuffer hs_data(request_length);
    hs_data.append(data_len_vi)\
           .append(packet_id)\
           .append(protocol_version)\
           .append(ip_len_vi)\
           .append(ip)\
           .append((uint8_t)(port >> 8)).append((uint8_t)(port & 0xFF))\
           .append(next_state);
    const char state_requestpack[2] = { 1, 0 };

    SOCKET sock;
    connectServer(sock, ip, port);

    send(sock, (const char*)hs_data.data(), hs_data.size(), 0);
    send(sock, state_requestpack, 2, 0);

    varInt pack_length, recv_vi;
    uint32_t recv_length;
    sock >> pack_length;
    recv(sock, (char*)&packet_id, 1, 0);
    sock >> recv_vi;
    recv_vi >> recv_length;
    //recv json
    char* buf = (char*)malloc(recv_length + 1);
    uint32_t recv_bytes = 0;
    while (recv_bytes < recv_length) {
        memset(buf, 0, recv_length + 1);
        recv_bytes += recv(sock, buf, recv_length, 0);
        info.append(buf);
    }
    closesocket(sock);
    return 0;
}

int getServerInfo(const std::string& ip, uint16_t port, Json::Value& info){
    std::string dat;
    int tmp = getServerInfo(ip, port, dat);
    if (tmp == 0) {
        //Ping success
        Json::Reader rd;
        if (rd.parse(dat, info)) {
            //Parse success
            return 0;
        }
        else {
            //Parse failed.
            return 1;
        }
    }
    return tmp;
}
