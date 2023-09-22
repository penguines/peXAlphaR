#include "cq_transcoder.h"

static const char* BASE64STR = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string encodeNL(std::string input) {
	int i = 0;
	while ((i = input.find("\\r\\n", i)) != std::string::npos) {
		input.replace(i, 4, "%0A");
	}
	i = 0;
	while ((i = input.find("\\n", i)) != std::string::npos) {
		input.replace(i, 2, "%0A");
	}
	i = 0;
	while ((i = input.find("\\\\", i)) != std::string::npos) {
		input.replace(i, 2, "\\");
	}
	return input;
}

std::string UrlUTF8(std::string str) {
	return UrlUTF8(str.c_str());
}

std::string GB2312ToUTF8(const char* str){
	int len = strlen(str) + 1;
	wchar_t* unicode = (wchar_t*)malloc(sizeof(wchar_t) * len);
	memset(unicode, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), unicode, len);

	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, 0, 0, 0, 0);
	if (!len)return str;
	char* utf8 = (char*)malloc(sizeof(char) * (len + 1));
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len + 1, 0, 0);
	utf8[len] = '\0';

	std::string result(utf8);
	free(unicode);
	free(utf8);
	return result;
}

std::string GB2312ToUTF8(const std::string& str){
	return GB2312ToUTF8(str.c_str());
}

std::string urlEncode(const char* str){
	std::string result;
	char str_tmp[4];
	while (*str != '\0') {
		if (isalnum(*str) == 0) {
			sprintf(str_tmp, "%%%02X", (BYTE)*str);
			result += str_tmp;
		}
		else {
			result += *str;
		}
		++str;
	}
	return result;
}

std::string urlEncode(const std::string& str){
	return urlEncode(str.c_str());
}

std::string utf8ToGB2312(const char* cstr_utf8){
	int len = MultiByteToWideChar(CP_UTF8, 0, cstr_utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	std::string result;

	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, cstr_utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) {
		delete[] wstr;
	}
	result = str;
	if (str) {
		delete[] str;
	}
	return result;
}

std::string utf8ToGB2312(const std::string& str_utf8){
	return utf8ToGB2312(str_utf8.c_str());
}

void base64Encode(std::vector<unsigned char>& str, std::string& result){
	int cnt = 0;
	unsigned char chr_tmp[3] = { 0 };
	char base64_chr[5] = { 0 };
	result.clear();
	for(auto iter = str.begin(); iter != str.end();iter++){
		chr_tmp[cnt] = *iter;
		cnt++;
		if (cnt == 3) {
			base64_chr[0] = BASE64STR[(chr_tmp[0] & 0xFC) >> 2];
			base64_chr[1] = BASE64STR[((chr_tmp[0] & 0x03) << 4) | ((chr_tmp[1] & 0xF0) >> 4)];
			base64_chr[2] = BASE64STR[((chr_tmp[1] & 0x0F) << 2) | ((chr_tmp[2] & 0xC0) >> 6)];
			base64_chr[3] = BASE64STR[chr_tmp[2] & 0x3F];
			result.append(base64_chr);
			cnt = 0;
		}
	}
	if (cnt == 1) {
		base64_chr[0] = BASE64STR[(chr_tmp[0] & 0xFC) >> 2];
		base64_chr[1] = BASE64STR[(chr_tmp[0] & 0x03) << 4];
		base64_chr[2] = '=';
		base64_chr[3] = '=';
		result.append(base64_chr);
	}
	else if (cnt == 2) {
		base64_chr[0] = BASE64STR[(chr_tmp[0] & 0xFC) >> 2];
		base64_chr[1] = BASE64STR[((chr_tmp[0] & 0x03) << 4) | ((chr_tmp[1] & 0xF0) >> 4)];
		base64_chr[2] = BASE64STR[(chr_tmp[1] & 0x0F) << 2];
		base64_chr[3] = '=';
		result.append(base64_chr);
	}
}

void toLowerCase(std::string& str){
	for (auto iter = str.begin(); iter != str.end(); iter++) {
		if (!islower(*iter)) {
			*iter = tolower(*iter);
		}
	}
}

std::string UrlUTF8(const char* strUnicode) {
	int len = strlen(strUnicode) + 1;
	wchar_t* unicode = (wchar_t*)malloc(sizeof(wchar_t) * len);
	memset(unicode, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strUnicode, strlen(strUnicode), unicode, len);

	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, 0, 0, 0, 0);
	if (!len)return strUnicode;
	char* utf8 = (char*)malloc(sizeof(char) * (len + 1));
	char* utf8Tmp = utf8;
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len + 1, 0, 0);
	utf8[len] = NULL;
	std::string strEncodeData;
	char strTmp[4];

	while (*utf8 != '\0') {
		sprintf(strTmp, "%%%02X", (BYTE)*utf8);
		strEncodeData += strTmp;
		++utf8;
	}
	free(unicode);
	free(utf8Tmp);
	return strEncodeData;
}

std::string UrlSpec(std::string input) {
	int i = 0;
	while ((i = input.find("%", i)) != std::string::npos) {
		input.replace(i, 1, "%25");
		i += 2;
	}
	i = 0;
	while ((i = input.find("+", i)) != std::string::npos) {
		input.replace(i, 1, "%2B");
	}
	i = 0;
	while ((i = input.find(" ", i)) != std::string::npos) {
		input.replace(i, 1, "+");
	}
	i = 0;
	while ((i = input.find("&", i)) != std::string::npos) {
		input.replace(i, 1, "%26");
	}
	i = 0;
	while ((i = input.find("=", i)) != std::string::npos) {
		input.replace(i, 1, "%3D");
	}
	i = 0;
	while ((i = input.find("&", i)) != std::string::npos) {
		input.replace(i, 1, "%26");
	}
	i = 0;
	while ((i = input.find("\r\n", i)) != std::string::npos) {
		input.replace(i, 2, "%0A");
	}
	i = 0;
	while ((i = input.find("\n", i)) != std::string::npos) {
		input.replace(i, 1, "%0A");
	}
	i = 0;
	for (i = 0; i < input.length(); i++) if (input.at(i) == '?') {
		input.replace(i, 1, "%3F");
	}
	return input;
}