#include "alpha_paths.h"

std::string getRunPath() {
	char filePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, filePath, MAX_PATH);
	std::string runpath = filePath;
	return runpath.substr(0, runpath.find_last_of("\\") + 1);
}

void getFilesA(std::string folder_path, std::vector<std::string>& files) {
	intptr_t handle_files = 0;
	struct _finddata_t file_info;
	std::string tmp;
	if ((handle_files = _findfirst(tmp.assign(folder_path).append("*").c_str(), &file_info)) != -1) {
		do {
			if ((file_info.attrib & _A_SUBDIR)) {
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
					getFilesA(tmp.assign(folder_path).append(file_info.name), files);
				}
			}
			else {
				files.push_back(tmp.assign(folder_path).append(file_info.name));
			}
		} while (_findnext(handle_files, &file_info) == 0);
		_findclose(handle_files);
	}
}

void getFilesB(std::string folder_path, std::vector<std::string>& files) {
	intptr_t handle_files = 0;
	struct _finddata_t file_info;
	std::string tmp;
	if ((handle_files = _findfirst(tmp.assign(folder_path).append("*").c_str(), &file_info)) != -1) {
		do {
			if ((file_info.attrib & _A_SUBDIR)) {
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
					getFilesB(tmp.assign(folder_path).append(file_info.name), files);
				}
			}
			else {
				files.push_back(tmp.assign(file_info.name));
			}
		} while (_findnext(handle_files, &file_info) == 0);
		_findclose(handle_files);
	}
}

void getSubFolders(std::string folder_path, std::vector<std::string>& sub_folders) {
	intptr_t handle_files = 0;
	struct _finddata_t file_info;
	std::string tmp;
	if ((handle_files = _findfirst(tmp.assign(folder_path).append("*").c_str(), &file_info)) != -1) {
		do {
			if ((file_info.attrib & _A_SUBDIR)) {
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
					sub_folders.push_back(tmp.assign(file_info.name));
				}
			}
		} while (_findnext(handle_files, &file_info) == 0);
		_findclose(handle_files);
	}
}

void getFileFullName(std::string file_path, std::string& file_name, char seg){
	size_t pos_tmp;
	if ((pos_tmp = file_path.find_last_of(seg)) != std::string::npos) {
		file_name = file_path.substr(pos_tmp + 1);
	}
	else {
		file_name = file_path;
	}
}

void getFileName(std::string file_path, std::string& file_name, char seg){
	getFileFullName(file_path, file_name, seg);
	file_name = file_name.substr(0, file_name.find_last_of('.'));
}

int createFolder(const std::string& folder_path){
	if (access(folder_path.c_str(), 0) != 0){
		return mkdir(folder_path.c_str());
	}
	return -1;
}

std::string getFileSuffix(const std::string& path){
	int pos = 0;
	if ((pos = path.find_last_of(".")) != std::string::npos) {
		return path.substr(pos + 1);
	}
	return std::string();
}
