#ifndef JSON_FILE_H
#define JSON_FILE_H

#include <vector>
#include <string>
#include <json/json.h>
#include <fstream>
#include <unordered_map>

class jsonFile {
public:
	std::string file_path;
	std::string file_name;
	std::string identifier;
	uint64_t	uid;

	Json::Value json;

	jsonFile(const std::string& fp = "",
			 const std::string& fn = "",
			 const std::string& id = "",
			 uint64_t			u64_id = 0);
	virtual ~jsonFile();
	
	int save();
	int load();
	void clear();
};

typedef jsonFile					JsonFile_t;
typedef jsonFile*					pJsonFile_t;
typedef std::vector<jsonFile>		jsonFileList;
typedef std::vector<pJsonFile_t>	pJsonFileList;
typedef std::unordered_map<uint64_t, jsonFile>		jsonFileMap;
typedef std::unordered_map<uint64_t, pJsonFileList>	jsonFileGroup;
typedef std::pair<uint64_t, pJsonFileList>			jsonFileGrpUnit;

#endif // !JSON_FILE_H
