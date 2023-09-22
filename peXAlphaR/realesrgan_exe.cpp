#include "realesrgan_exe.h"

static std::string alpha_runpath;

int animeImgEnhance(std::string img_full_path, std::string& output_full_path){
	std::string realesrgan_path(alpha_runpath);
	std::string output_file;
	realesrgan_path.append(REALESRGAN_PATH);
	getFileFullName(img_full_path, output_file, '/');
	char cmd[COMMAND_LENGTH];
	FILE* fp;
	output_full_path.assign(realesrgan_path).append(output_file);
	sprintf(cmd, REALESRGAN_CMD, realesrgan_path.c_str(), img_full_path.c_str(), output_full_path.c_str());
	if ((fp = _popen(cmd, "r")) != NULL) {
		_pclose(fp);
	}
	else {
		return 0;
	}
	return 1;
}

int imgEnhanceProc(std::string img_file, std::string reply_id, std::string sender_id, int msg_type){
	std::string img_full_path, output_img_path;
	CQJsonMsg reply_msg;

	if (img_file.empty()) {
		return 0;
	}

	size_t img_size;
	getImage(img_file, img_full_path, img_size);
	if ((img_size / 1024) <= IMG_MAX_INPUT_SIZE_KB) {
		animeImgEnhance(img_full_path, output_img_path);
		switch (msg_type) {
		case MSG_GROUP:
			reply_msg.append(CQJmsg::at(sender_id));
			reply_msg.append(CQJmsg::image("file:///" + output_img_path));
			sendGroupMsg(reply_id, reply_msg.getJson());
			break;
		case MSG_PRIVATE:
			reply_msg.append(CQJmsg::image("file:///" + output_img_path));
			sendPrivateMsg(reply_id, reply_msg.getJson());
			break;
		}
		remove(output_img_path.c_str());
		return 1;
	}
	else {
		switch(msg_type) {
		case MSG_GROUP:
			reply_msg.append(CQJmsg::at(sender_id));
			reply_msg.append(GB2312ToUTF8("图片过大！"));
			sendGroupMsg(reply_id, reply_msg.getJson());
			break;
		case MSG_PRIVATE:
			reply_msg.append(GB2312ToUTF8("图片过大！"));
			sendPrivateMsg(reply_id, reply_msg.getJson());
			break;
		}
		return 0;
	}
}

int imgEnhance(CQmsg& msg){
	if (!checkPermission(msg, IMG_ENHANCE_PERMISSION)) {
		return 0;
	}

	const Json::Value& tmp = msg.content;
	std::string str_tmp;
	int cnt = 0;
	if (tmp.isArray()) {
		for (int i = 0; i < tmp.size(); i++) {
			loadStringByKeyword("type", tmp[i], str_tmp);
			if (str_tmp == "image") {
				if (!tmp[i].isMember("data")) {
					continue;
				}
				loadStringByKeyword("file", tmp[i]["data"], str_tmp);
				std::thread thread_tmp(imgEnhanceProc, str_tmp, getMsgReplyID(msg), getMsgSenderID(msg), msg.msg_type);
				thread_tmp.detach();
				cnt++;
			}
		}
	}
	if (cnt) {
		sendReply(msg, "正在增强" + std::to_string(cnt) + "张图片……", 0);
		return 1;
	}
	else {
		sendReply(msg, "搞毛？", 0);
		return 0;
	}
	return 0;
}

void register_imgEnhance(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = imgEnhance;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back("hd");
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.name = "高清增强";
	event_tmp.tag.permission = IMG_ENHANCE_PERMISSION;
	event_tmp.tag.example = "hd[图片]";
	event_tmp.tag.description = "基于realesrgan对指定图片进行高清增强。";

	event_list.push_back(event_tmp);
}

void realesrganExe::setRunPath(const std::string& run_path){
	alpha_runpath = run_path;
}
