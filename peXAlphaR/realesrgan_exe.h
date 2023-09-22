#ifndef REALESRGAN_EXE_H
#define REALESRGAN_EXE_H

#include <stdio.h>
#include <string>
#include <thread>
#include "cq_api_post.h"
#include "alpha_paths.h"
#include "alpha_permissions.h"

#define COMMAND_LENGTH	4096
#define REALESRGAN_PATH "realesrgan\\"
#define REALESRGAN_CMD	"%srealesrgan-ncnn-vulkan.exe -i %s -o %s realesrgan-x4plus"
#define IMG_ENHANCE_PERMISSION	0
#define IMG_MAX_INPUT_SIZE_KB	1024

namespace realesrganExe {
	void setRunPath(const std::string& run_path);
}

int animeImgEnhance(std::string img_full_path, std::string& output_full_path);
int imgEnhanceProc(std::string img_file, std::string reply_id, std::string sender_id, int msg_type);
int imgEnhance(CQmsg& msg);
void register_imgEnhance(std::vector<CQMsgEvent>& event_list);

#endif // REALESRGAN_EXE_H
