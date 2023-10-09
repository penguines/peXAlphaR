#ifndef ALPHA_TYPES_H
#define ALPHA_TYPES_H

#include <cstdint>
#include <malloc.h>

#define VARINT_CONTINUE_BIT	0x80
#define VARINT_SEGMENT_BITS	0x7F

class varInt {
public:
	varInt();
	varInt(const varInt& val);
	varInt(uint32_t val);

	~varInt();

	uint8_t size() const;
	const uint8_t* value() const;

	varInt& operator=(const varInt& val);
	varInt& operator=(uint32_t val);

private:
	uint8_t* data;
	uint8_t sz;
};

static void varIntDataCopy(const uint8_t* src, uint8_t* dst, uint8_t n);

#endif // !ALPHA_TYPES_H
