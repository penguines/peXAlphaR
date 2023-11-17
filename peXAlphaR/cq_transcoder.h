#ifndef CQ_TRANSCODER_H
#define CQ_TRANSCODER_H

#include <string>
#include <vector>
#include <WinSock2.h>

//If all const strings are transcoded to UTF-8
//Please [#define DEFAULT_CHARSET_UTF8] instead.
#define DEFAULT_CHARSET_GBK

#ifndef DEFAULT_CHARSET_UTF8
//if default charset is NOT utf8

#define _U(_str) GB2312ToUTF8(_str)
#define _G(_str) _str
#define __IS_UTF8__	0
#define __IS_GBK__	1

#else

#define _U(_str) _str
#define _G(_str) utf8ToGB2312(_str) 
#define __IS_UTF8__	1
#define __IS_GBK__	0

#endif

std::string encodeNL(std::string input);

std::string UrlUTF8(const char* str);
std::string UrlUTF8(std::string str);
std::string GB2312ToUTF8(const char* str);
std::string GB2312ToUTF8(const std::string& str);
std::string urlEncode(const char* str);
std::string urlEncode(const std::string& str);

std::string UrlSpec(std::string input);

std::string utf8ToGB2312(const char* cstr_utf8);
std::string utf8ToGB2312(const std::string& str_utf8);

void base64Encode(std::vector<unsigned char>& str, std::string& result);

void base64Decode(const std::string& base64_str, std::vector<unsigned char>& result);

void toLowerCase(std::string& str);

#endif // !CQ_TRANSCODER_H

