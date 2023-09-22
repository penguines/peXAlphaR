#include "json_file.h"

jsonFile::jsonFile(const std::string& fp, const std::string& fn, const std::string& id, uint64_t u64_id){
	this->file_path = fp;
	this->file_name = fn;
	this->identifier = id;
	this->uid = u64_id;
}

jsonFile::~jsonFile(){

}

int jsonFile::save(){
	std::string file(file_path);
	file.append(file_name);
	std::ofstream file_wr;
	file_wr.open(file, std::iostream::out);
	if (!file_wr.is_open()) {
		return 0;
	}
	file_wr << this->json.toStyledString();
	file_wr.close();
}

int jsonFile::load(){
	std::string file(file_path);
	file.append(file_name);

	std::ifstream file_rd;
	file_rd.open(file, std::iostream::in);
	if (!file_rd.is_open()) {
		return 0;
	}
	std::string json_str((std::istreambuf_iterator<char>(file_rd)), (std::istreambuf_iterator<char>()));
	file_rd.close();

	Json::Reader json_rd;
	this->json.clear();
	if (json_rd.parse(json_str, this->json)) {
		return 1;
	}
	else {
		return 0;
	}
}

void jsonFile::clear(){
	this->file_path.clear();
	this->file_name.clear();
	this->identifier.clear();
	this->uid = 0;
	this->json.clear();
}