#include "alpha_types.h"

varInt::varInt(){
	this->data = nullptr;
	this->sz = 0;
}

varInt::varInt(const varInt& val){
    this->sz = val.size();
    if (this->sz == 0) {
        this->data = nullptr;
        return;
    }
    this->data = (uint8_t*)malloc(this->sz);
    varIntDataCopy(val.value(), this->data, this->sz);
}

varInt::varInt(uint32_t val){
    if (val == 0) {
        this->sz = 1;
        this->data = (uint8_t*)malloc(1);
        *(this->data) = 0;
        return;
    }
    this->sz = 0;
    uint32_t tmp = val;
    while (tmp & VARINT_SEGMENT_BITS) {
        tmp >>= 7;
        this->sz++;
    }
    this->data = (uint8_t*)malloc(this->sz);
    uint8_t i = 0;
    for (; i < this->sz - 1; i++) {
        (this->data)[i] = (val & VARINT_SEGMENT_BITS);
        (this->data)[i] |= VARINT_CONTINUE_BIT;
        val >>= 7;
    }
    (this->data)[i] = val;
}

varInt::~varInt(){
    if (this->data != nullptr) {
        free(this->data);
    }
}

uint8_t varInt::size() const{
	return this->sz;
}

const uint8_t* varInt::value() const{
    return this->data;
}

varInt& varInt::operator=(const varInt& val){
    uint8_t sz_val = val.size();
    if (sz_val != this->sz) {
        uint8_t* ptr;
        if ((ptr = (uint8_t*)realloc(this->data, sz_val)) != nullptr) {
            this->sz = sz_val;
            this->data = ptr;
        }
        else {
            return *this;
        }
    }
    varIntDataCopy(val.value(), this->data, this->sz);
    return *this;
}

varInt& varInt::operator=(uint32_t val){
    uint8_t sz_val = 0;
    uint32_t tmp = val;
    while (tmp & VARINT_SEGMENT_BITS) {
        tmp >>= 7;
        sz_val++;
    }
    if (sz_val != this->sz) {
        uint8_t* ptr;
        if ((ptr = (uint8_t*)realloc(this->data, sz_val)) != nullptr) {
            this->sz = sz_val;
            this->data = ptr;
        }
        else {
            return *this;
        }
    }
    for (sz_val = 0; sz_val < this->sz - 1; sz_val++) {
        (this->data)[sz_val] = (val & VARINT_SEGMENT_BITS);
        (this->data)[sz_val] |= VARINT_CONTINUE_BIT;
        val >>= 7;
    }
    (this->data)[sz_val] = val;
	return *this;
}

void varIntDataCopy(const uint8_t* src, uint8_t* dst, uint8_t n){
    for (uint8_t i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}
