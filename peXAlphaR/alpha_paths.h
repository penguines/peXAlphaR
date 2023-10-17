#ifndef ALPHA_PATHS_H
#define ALPHA_PATHS_H

#include <string>
#include <vector>
#include <io.h>
#include <Windows.h>
#include <direct.h>

std::string getRunPath();
//get full filepath & name list with suffix
void getFilesA(std::string folder_path, std::vector<std::string>& files);
//get filename list with suffix
void getFilesB(std::string folder_path, std::vector<std::string>& files);
void getSubFolders(std::string folder_path, std::vector<std::string>& sub_folders);
//get filename with suffix
void getFileFullName(std::string file_path, std::string& file_name, char seg);
//get filename without suffix
void getFileName(std::string file_path, std::string& file_name, char seg = '\\');
//return 0: success; return -1: fail.
int createFolder(const std::string& folder_path);

std::string getFileSuffix(const std::string& path);

#endif // !ALPHA_PATHS_H
