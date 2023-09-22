#include "alpha_basic_event.h"

using namespace CQJmsg;

static std::string img_folder;
static std::string img_folder_f;
static std::string sauce_api_key;
static alpha_basicEvents::presentsList ava_presents;
static jsonFileGroup* active_grp_users;

static size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream) {
	std::string* str = (std::string*)stream;
	(*str).append((char*)ptr, size * nmemb);
	return size * nmemb;
}

int dailySignIn(CQmsg& msg) {
	if (!checkPermission(msg, ALPHA_DAILY_SINGIN_PERMISSION)) {
		return 0;
	}

	pCQuser_t msg_sender = getMsgSender(msg);
	pJsonFile_t data_tmp = msg_sender->customed_user_data;
	if (data_tmp == nullptr) {
		return 0;
	}
	time_t time_tmp;
	CQJsonMsg signMsg;
	int last_signed = 0;
	int cur_date = 0;
	int cur_signs = 0;
	int cur_continuous_sign = 0;
	int max_continuous_sign = 0;
	int present_times = 0;
	int present_times_day = 1;
	time(&time_tmp);
	time_tmp += 3600 * 8;
	cur_date = time_tmp / 86400;

	signMsg += CQJmsg::at(msg_sender->id);
	if (!loadIntByKeyword("last_signed", data_tmp->json, last_signed)) {
		data_tmp->json["last_signed"] = cur_date;
		data_tmp->json["cur_signs"] = 1;
		data_tmp->json["cur_continuous_signs"] = 1;
		data_tmp->json["max_continuous_signs"] = 1;
		data_tmp->save();

		if (msg.msg_type == MSG_GROUP) {
			CQGroupMsg& grpmsg_tmp = static_cast<CQGroupMsg&>(msg);
			uint64_t group_uid = grpmsg_tmp.group->u64_id;
			auto it = active_grp_users->find(group_uid);
			if (it != active_grp_users->end()) {
				it->second.emplace_back(grpmsg_tmp.sender->customed_user_data);
			}
			else {
				active_grp_users->insert(jsonFileGrpUnit(group_uid, pJsonFileList()));
				active_grp_users->find(group_uid)->second.emplace_back(grpmsg_tmp.sender->customed_user_data);
			}
		}

		signMsg += _U(" 口_口");
		sendReply(msg, signMsg.getJson());
		return 1;
	}
	else {
		if (cur_date != last_signed) {
			loadIntByKeyword("cur_signs", data_tmp->json, cur_signs, 1);
			loadIntByKeyword("cur_continuous_signs", data_tmp->json, cur_continuous_sign, 1);
			loadIntByKeyword("max_continuous_signs", data_tmp->json, max_continuous_sign, 1);
			loadIntByKeyword("present_draw", data_tmp->json, present_times, 0);
			//每日增加的抽礼物次数
			present_times_day = randomInt(1, 3);
			present_times += present_times_day;
			cur_signs++;
			data_tmp->json["cur_signs"] = cur_signs;
			if (cur_date == last_signed + 1) {
				cur_continuous_sign++;
			}
			else {
				cur_continuous_sign = 1;
			}
			data_tmp->json["cur_continuous_signs"] = cur_continuous_sign;

			if (cur_continuous_sign > max_continuous_sign) {
				max_continuous_sign = cur_continuous_sign;
				data_tmp->json["max_continuous_signs"] = max_continuous_sign;
			}
			data_tmp->json["last_signed"] = cur_date;
			data_tmp->json["present_draw"] = present_times;
			data_tmp->save();

			//回复消息
			std::string signs_tmp(_G("【当前连续签到天数: "));
			signs_tmp.append(std::to_string(cur_continuous_sign)).append(_G("】\n【最大连续签到天数: "));
			signs_tmp.append(std::to_string(max_continuous_sign)).append(_G("】\n【总签到天数: "));
			signs_tmp.append(std::to_string(cur_signs)).append(_G("】\n#今日抽礼物次数+"));
			signs_tmp.append(std::to_string(present_times_day)).append("#");
			signMsg += _G(" 签上到啦！！！");
			signMsg += CQJmsg::image(img_folder_f + ALPHA_SIGN_IMG_1);
			signMsg += signs_tmp;
			signMsg.textToUTF8();
			sendReply(msg, signMsg.getJson());
			return 1;
		}
		else {
			signMsg += _G(" 您今天已经签过到了...");
			signMsg += CQJmsg::image(img_folder_f + ALPHA_SIGN_IMG_2);
			signMsg.textToUTF8();
			sendReply(msg, signMsg.getJson());
			return 0;
		}
	}
	return 0;
}

void register_dailySignIn(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = dailySignIn;
	//EVENT_GROUP set to EVENT_ALL, 2022-07-30
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back(_G("签到"));
	event_tmp.trig_msg.emplace_back(_G("/签到"));
	event_tmp.trig_msg.emplace_back(_G("#签到"));
	event_tmp.msg_codetype = CODE_UTF8;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = ALPHA_DAILY_SINGIN_PERMISSION;
	event_tmp.tag.name = _G("每日签到");
	event_tmp.tag.example = _G("签到(/签到;#签到)");
	event_tmp.tag.description = _G("每日签到。");

	event_list.push_back(event_tmp);
}


int atSB(CQmsg& msg) {
	CQGroupMsg* group_msg = (CQGroupMsg*)&msg;
	CQJsonMsg jmsg_tmp;
	jmsg_tmp.append(at(group_msg->sender->id)).append(image(img_folder_f + "atsb.jpg"));
	sendGroupMsg(group_msg->group->id, jmsg_tmp.getJson());
	return 0;
}

int condition_atSB(CQmsg& msg) {
	std::string str_tmp;
	if (!msg.content.isArray()) {
		return 0;
	}
	for (int i = 0; i < msg.content.size(); i++) {
		loadStringByKeyword("type", msg.content[i], str_tmp);
		if (str_tmp == "at") {
			if (msg.content[i].isMember("data")) {
				loadStringByKeyword("qq", msg.content[i]["data"], str_tmp);
				if (str_tmp == msg.self_id) {
					return 1;
				}
			}
		}
	}
	return 0;
}

void register_atSB(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = atSB;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.custom_condition = condition_atSB;
	event_tmp.trig_type = TRIG_CUSTOMIZE;
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.name = "atsb";

	event_list.push_back(event_tmp);
}


void imageSearch(const std::string& img_url, std::string& response) {
	static const std::string post_url = "https://saucenao.com/search.php?";
	static std::string post_full_url = post_url + "output_type=2&numres=1&minsim=80!&dbmask=8191&api_key=" + sauce_api_key;

	std::string m_data = "-----------------------------54382956620782\n"
		"Content-Disposition: form-data; name=\"file\"; filename=\"\"\n"
		"Content-Type: application/octet-stream\n"
		"\n"
		"\n"
		"-----------------------------54382956620782\n"
		"Content-Disposition: form-data; name=\"url\"\n"
		"\n";
	m_data += img_url + "\n";
	m_data += "-----------------------------54382956620782\n"
		"Content-Disposition: form-data; name=\"frame\"\n"
		"\n"
		"1\n"
		"-----------------------------54382956620782\n"
		"Content-Disposition: form-data; name=\"hide\"\n"
		"\n"
		"0\n"
		"-----------------------------54382956620782\n"
		"Content-Disposition: form-data; name=\"database\"\n"
		"\n"
		"999\n"
		"-----------------------------54382956620782--\n";

	CURL* curl = curl_easy_init();
	// res code  
	CURLcode res;
	struct curl_slist* headers = NULL;
	struct curl_httppost* postdata = NULL;
	struct curl_httppost* postdataLst = NULL;
	headers = curl_slist_append(headers, "Host: saucenao.com");
	headers = curl_slist_append(headers, "Content-Type: multipart/form-data; boundary=---------------------------54382956620782");
	if (curl) {
		// set params  
		curl_easy_setopt(curl, CURLOPT_POST, 1L); // post req  
		curl_easy_setopt(curl, CURLOPT_URL, post_full_url.c_str()); // url  
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		//curl_easy_setopt(curl, CURLOPT_HTTPPOST, postdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_data.c_str()); // params  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		// start req  
		res = curl_easy_perform(curl);
		//std::cout << utf8ToGB2312(response) << std::endl;
	}
	// release curl  
	curl_easy_cleanup(curl);
}

void imageSearchProc(std::string image_url, std::string reply_id, std::string at_id, int msg_type) {
	CQJsonMsg result_msg;
	std::string result_msg_str;
	std::string search_result;
	std::string img_sim, img_url, img_src, img_artist, img_artist_id;
	std::string img_title, img_src_url, img_id;

	imageSearch(image_url, search_result);
	if (search_result.empty()) {
		if (msg_type == MSG_GROUP) {
			sendGroupMsg(reply_id, "搜图服务器无响应。", __IS_UTF8__);
		}
		else {
			sendPrivateMsg(reply_id, "搜图服务器无响应。", __IS_UTF8__);
		}
		return;
	}

	imgSearchAnsJson(search_result,
		img_sim,
		img_url,
		img_src,
		img_artist,
		img_artist_id,
		img_title,
		img_src_url,
		img_id);

	if (!img_url.empty()) {
		result_msg_str.append(_U("\n相似度: ")).append(img_sim).append("\%");
		result_msg_str.append(_U("\n来源: ")).append(img_src);
		result_msg_str.append(_U("\n标题: ")).append(img_title);
		result_msg_str.append(_U("\n作者: ")).append(img_artist);
		result_msg_str.append(_U("\n作者id: ")).append(img_artist_id);
		result_msg_str.append(_U("\n图片id: ")).append(img_id);
		result_msg_str.append(_U("\n图片链接: ")).append(img_src_url);
		result_msg_str.append("\n[SauceNAO]");

		result_msg += CQJmsg::at(at_id);
		result_msg += _U(" 搜索结果:\n");
		result_msg += CQJmsg::image(img_url);
		result_msg += result_msg_str;
	}
	else {
		result_msg += CQJmsg::at(at_id);
		result_msg += _U("未找到相似图片。");
	}

	if (msg_type == MSG_GROUP) {
		sendGroupMsg(reply_id, result_msg.getJson());
	}
	else {
		sendPrivateMsg(reply_id, result_msg.getJson());
	}
}

void imgSearchAnsJson(const std::string& resultJson,
	std::string& similarity,
	std::string& imgUrl,
	std::string& src,
	std::string& artist,
	std::string& artist_id,
	std::string& title,
	std::string& srcUrl,
	std::string& imgId)
{
	Json::Reader rd;
	Json::Value result, resultHeader, resultData;
	double simtmp;
	if (rd.parse(resultJson, result)) {
		resultHeader = result["results"][0]["header"];
		resultData = result["results"][0]["data"];
		similarity = resultHeader["similarity"].asString();
		simtmp = atof(similarity.c_str());
		if (simtmp < 40) {
			imgUrl = "";
			return;
		}
		imgUrl = resultHeader["thumbnail"].asString();
		srcUrl = resultData["ext_urls"][0].asString();
		if (resultData.isMember("title")) title = resultData["title"].asString();
		else title = resultData["characters"].asString();
		if (resultData.isMember("member_id")) {
			artist_id = resultData["member_id"].asString();
			artist = resultData["member_name"].asString();
			if (resultData.isMember("pixiv_id")) {
				src = "Pixiv";
				imgId = resultData["pixiv_id"].asString();
			}
			else {
				src = "Seiga";
				imgId = resultData["pixiv_id"].asString();
			}
		}
		else if (resultData.isMember("creator")) {
			artist = resultData["creator"].asString();
			srcUrl = resultData["source"].asString();
			if (resultData.isMember("danbooru_id")) {
				src = "Danbooru";
				imgId = resultData["danbooru_id"].asString();
			}
		}
	}
}

int imgSearch(CQmsg& msg) {
	if (sauce_api_key.empty()) {
		PRINTLOG("无法搜图: 无SauceNAO API key.");
		return 0;
	}
	if (!checkPermission(msg, ALPHA_IMGSEARCH_PERMISSION)) {
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
				loadStringByKeyword("url", tmp[i]["data"], str_tmp);
				std::thread thread_tmp(imageSearchProc, str_tmp, getMsgReplyID(msg), getMsgSenderID(msg), msg.msg_type);
				thread_tmp.detach();
				cnt++;
			}
		}
	}
	if (cnt) {
		sendReply(msg, "正在搜索" + std::to_string(cnt) + "张图片……", __IS_UTF8__);
		return 1;
	}
	else {
		sendReply(msg, "搞毛？", __IS_UTF8__);
		return 0;
	}
}

void register_imgSearch(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = imgSearch;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back(_G("搜图"));
	event_tmp.trig_msg.emplace_back(_G("/搜图"));
	event_tmp.trig_msg.emplace_back(_G("#搜图"));
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.name = _G("搜图");
	event_tmp.tag.permission = ALPHA_IMGSEARCH_PERMISSION;
	event_tmp.tag.example = _G("搜图[图片]");
	event_tmp.tag.description = _G("基于SauceNAO引擎搜索指定图片。");

	event_list.push_back(event_tmp);
}

int drawPresents(CQmsg& msg) {
	if (!checkPermission(msg, DRAW_PRESENTS_PERMISSION)) {
		return 0;
	}

	pCQuser_t user_tmp = getMsgSender(msg);
	CQJsonMsg reply_msg;
	std::string reply_str;
	int present_times;
	loadIntByKeyword("present_draw", user_tmp->customed_user_data->json, present_times, 3);
	reply_msg.append(CQJmsg::at(user_tmp->id));
	if (present_times <= 0) {
		reply_msg.append(_U("抽礼物次数不足！"));
		sendReply(msg, reply_msg.getJson());
		return 0;
	}
	else {
		present_times--;
		user_tmp->customed_user_data->json["present_draw"] = present_times;
	}

	double rand_dbl = randomDouble(0, 100);
	size_t present_size = ava_presents.percent_sum.size();
	for (int i = 0; i < present_size; i++) {
		if (rand_dbl < ava_presents.percent_sum[i]) {
			Json::Value& present_tmp = ava_presents.presents.json[i];
			editUserPresent(*user_tmp, present_tmp, 1, presentEdit::PRESENT_ADD);
			user_tmp->customed_user_data->save();
			reply_str.assign(_U("今天抽到了【")).append(present_tmp["name"].asString()).append(_U("】!"));
			reply_msg.append(reply_str);
			reply_msg.append(CQJmsg::image(img_folder_f + present_tmp["image"].asString()));
			reply_str.assign("\n").append(_U("——"));
			reply_str.append("\"").append(present_tmp["description"].asString()).append("\"");
			reply_str.append(_U("\n剩余抽礼物次数: ") + std::to_string(present_times));
			reply_msg.append(reply_str);
			sendReply(msg, reply_msg.getJson());
			return i;
		}
	}
	user_tmp->customed_user_data->save();
	reply_msg.append(_U(" 今天什么都没抽到！\n剩余抽礼物次数: ") + std::to_string(present_times));
	sendReply(msg, reply_msg.getJson());
	return 0;
}

void register_drawPresents(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = drawPresents;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back(_G("抽礼物"));
	event_tmp.trig_msg.emplace_back(_G("/抽礼物"));
	event_tmp.trig_msg.emplace_back(_G("#抽礼物"));
	event_tmp.msg_codetype = CODE_UTF8;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = DRAW_PRESENTS_PERMISSION;
	event_tmp.tag.name = _G("抽礼物");
	event_tmp.tag.example = _G("抽礼物(/抽礼物;#抽礼物)");
	event_tmp.tag.description = _G("抽礼物，每日签到可获得一次抽取机会。");

	event_list.push_back(event_tmp);
}

int showPresents(CQmsg& msg) {
	if (!checkPermission(msg, SHOW_PRESENTS_PERMISSION)) {
		return 0;
	}

	pCQuser_t user_tmp = getMsgSender(msg);
	CQJsonMsg reply_msg;
	std::string reply_str;

	reply_msg.append(CQJmsg::at(user_tmp->id));
	if (!user_tmp->customed_user_data->json.isMember("presents")) {
		reply_msg.append(_U("\n米椰奶米椰奶~"));
		sendReply(msg, reply_msg.getJson());
		return 0;
	}
	const Json::Value& json_tmp = user_tmp->customed_user_data->json["presents"];
	int presents_size = json_tmp.size();
	int flag = 0;
	int num_tmp;
	reply_str.assign(_U("当前礼物列表:\n"));
	for (int i = 0; i < presents_size; i++) {
		num_tmp = json_tmp[i]["num"].asInt();
		if (num_tmp > 0) {
			reply_str.append(json_tmp[i]["name"].asString()).append(" x");
			reply_str.append(std::to_string(num_tmp)).append("\n");
			flag++;
		}
	}
	if (flag == 0) {
		reply_str.append(_U("米椰奶米椰奶~"));
	}
	reply_msg.append(reply_str);
	sendReply(msg, reply_msg.getJson());

	return 1;
}

void register_showPresents(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = showPresents;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back(_G("礼物列表"));
	event_tmp.trig_msg.emplace_back(_G("/礼物列表"));
	event_tmp.trig_msg.emplace_back(_G("#礼物列表"));
	event_tmp.msg_codetype = CODE_UTF8;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = SHOW_PRESENTS_PERMISSION;
	event_tmp.tag.name = _G("礼物列表");
	event_tmp.tag.example = _G("礼物列表(/礼物列表;#礼物列表)");
	event_tmp.tag.description = _G("查看已有的礼物。");

	event_list.push_back(event_tmp);
}

int usePresents(CQmsg& msg) {
	if (!checkPermission(msg, USE_PRESENTS_PERMISSION)) {
		return 0;
	}
	//解析送礼物指令
	std::regex reg(_U(USE_PRESENTS_REGEX));
	std::smatch reg_result;
	std::regex_search(msg.text(), reg_result, reg);
	std::string present_name;
	std::string str_tmp;
	int present_usenum, present_curnum;
	str_tmp = reg_result[3].str();
	if (str_tmp.empty()) {
		present_usenum = 1;
	}
	else {
		present_usenum = atoi(str_tmp.c_str());
	}
	present_name = reg_result[4].str();

	pCQuser_t user_tmp = getMsgSender(msg);
	Json::Value& user_data = user_tmp->customed_user_data->json;
	CQJsonMsg reply_msg(CQJmsg::at(user_tmp->id));
	std::string reply_str;

	int cur_index;
	const Json::Value& present_tmp = ava_presents.presents.json;
	cur_index = getJsonIndexByKeyword("name", present_name, present_tmp);
	if (cur_index == -1) {
		cur_index = getJsonIndexByKeyword("name", present_name, present_tmp, JSON_FIND_CONTAIN);
		if (cur_index == -1) {
			/*
			reply_msg.append(_U("没有这样的礼物!"));
			sendReply(msg, reply_msg.getJson());
			*/
			return 0;
		}
	}
	const Json::Value& cur_present = present_tmp[cur_index];
	present_curnum = editUserPresent(*user_tmp, cur_present, present_usenum, PRESENT_CONSUME);
	if (present_curnum < 0) {
		reply_msg.append(_U("礼物数量不足，无法使用。"));
		sendReply(msg, reply_msg.getJson());
		return 0;
	}

	int present_favor, user_favor;
	loadIntByKeyword("favor", cur_present, present_favor);
	loadIntByKeyword("favor", user_data, user_favor, 0);
	loadStringByKeyword("use_msg", cur_present, str_tmp);
	present_favor *= present_usenum;
	user_favor += present_favor;
	user_data["favor"] = user_favor;
	user_tmp->customed_user_data->save();

	reply_str.assign(_U("成功使用")).append(cur_present["name"].asString()).append("*");
	reply_str.append(std::to_string(present_usenum)).append(_U("!\n好感度+("));
	reply_str.append(std::to_string(present_favor)).append(_U(")\n当前好感度: "));
	reply_str.append(std::to_string(user_favor)).append("\n");
	reply_str.append(str_tmp);
	reply_msg.append(reply_str);
	sendReply(msg, reply_msg.getJson());
	return 1;
}

void register_usePresents(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = usePresents;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = _U(USE_PRESENTS_REGEX);
	event_tmp.msg_codetype = CODE_UNICODE;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = USE_PRESENTS_PERMISSION;
	event_tmp.tag.name = _G("送礼物");
	event_tmp.tag.example = _G("送/吃[数量]个[礼物名称]");
	event_tmp.tag.description = _G("使用已有的礼物。");

	event_list.push_back(event_tmp);
}

int throwPresents(CQmsg& msg){
	if (!checkPermission(msg, THROW_PRESENTS_PERMISSION)) {
		return 0;
	}
	//解析指令
	std::regex reg(_U(THROW_PRESENTS_REGEX));
	std::smatch reg_result;
	std::regex_search(msg.text(), reg_result, reg);
	std::string present_name;
	std::string str_tmp;
	int present_usenum, present_curnum;
	str_tmp = reg_result[2].str();
	if (str_tmp.empty()) {
		present_usenum = 1;
	}
	else {
		present_usenum = atoi(str_tmp.c_str());
	}
	present_name = reg_result[3].str();
	CQGroupMsg& group_msg = static_cast<CQGroupMsg&>(msg);
	pCQuser_t user_tmp = getMsgSender(msg);
	Json::Value& user_data = user_tmp->customed_user_data->json;
	CQJsonMsg reply_msg(CQJmsg::at(user_tmp->id));
	std::string reply_str;

	int cur_index;
	const Json::Value& present_tmp = ava_presents.presents.json;
	cur_index = getJsonIndexByKeyword("name", present_name, present_tmp);
	if (cur_index == -1) {
		cur_index = getJsonIndexByKeyword("name", present_name, present_tmp, JSON_FIND_CONTAIN);
		if (cur_index == -1) {
			//无法找到对应礼物
			reply_msg.append(_U(" 【数据删除】"));
			sendGroupMsg(group_msg.group->id, reply_msg.getJson());
			return 0;
		}
	}
	const Json::Value& cur_present = present_tmp[cur_index];
	//索敌
	auto cur_grp_users = active_grp_users->find(group_msg.group->u64_id);
	if (cur_grp_users == active_grp_users->end()) {
		reply_msg.append(_U(" 警告：无法索敌..."));
		sendGroupMsg(group_msg.group->id, reply_msg.getJson());
		return 0;
	}
	std::regex reg_at(STYLED_AT_REGEX);
	uint64_t uid_tmp;
	pJsonFileList& users_data = cur_grp_users->second;
	pJsonFile_t dst_user;
	if (std::regex_search(msg.toStyledText(), reg_result, reg_at)) {
		//手动at选取目标
		bool is_find = false;
		str_tmp = reg_result[1].str();
		uid_tmp = strtoull(reg_result[1].str().c_str(), NULL, 10);
		for (auto iter = users_data.begin(); iter != users_data.end(); iter++) {
			if (uid_tmp == (*iter)->uid) {
				dst_user = *iter;
				is_find = true;
				//PRINTLOG("Target locked.");
				break;
			}
		}
		if (is_find == false) {
			reply_msg.append(_U(" 警告：无法选取..."));
			sendGroupMsg(group_msg.group->id, reply_msg.getJson());
			return 0;
		}
	}
	else {
		//随机目标
		int sz = users_data.size();
		if (sz != 0) {
			dst_user = users_data[randomInt(0, sz - 1)];
		}
		else {
			//若群组中无其他活跃成员，则默认命中自己
			dst_user = user_tmp->customed_user_data;
		}
	}
	present_curnum = editUserPresent(*user_tmp, cur_present, present_usenum, PRESENT_CONSUME);
	if (present_curnum < 0) {
		reply_msg.append(_U(" 礼物数量不足，无法使用。"));
		sendGroupMsg(group_msg.group->id, reply_msg.getJson());
		return 0;
	}
	user_tmp->customed_user_data->save();
	int present_favor, dst_user_favor;
	loadIntByKeyword("favor", cur_present, present_favor);
	loadIntByKeyword("favor", dst_user->json, dst_user_favor, 0);
	loadStringByKeyword("use_msg", cur_present, str_tmp);
	present_favor *= present_usenum;
	dst_user_favor -= present_favor;
	dst_user->json["favor"] = dst_user_favor;
	dst_user->save();

	reply_msg.append(_U(" 向")).append(CQJmsg::at(dst_user->identifier));
	reply_str.append(_U(" 扔出了")).append(std::to_string(present_usenum));
	reply_str.append(_U("个")).append(cur_present["name"].asString()).append("\n");
	reply_str.append(_U("目标好感度下降了")).append(std::to_string(present_favor));
	reply_str.append("! (Remaining: ").append(std::to_string(dst_user_favor)).append(")");
	reply_msg.append(reply_str);
	sendGroupMsg(group_msg.group->id, reply_msg.getJson());
	return 1;
}

void register_throwPresents(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = throwPresents;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = _U(THROW_PRESENTS_REGEX);
	event_tmp.msg_codetype = CODE_UNICODE;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = THROW_PRESENTS_PERMISSION;
	event_tmp.tag.name = _G("扔礼物");
	event_tmp.tag.example = _G("扔[数量]个[礼物名称](可选:[@指定目标])");
	event_tmp.tag.description = _G("扔出已有的礼物。");

	event_list.push_back(event_tmp);
}

int editUserPresent(CQuser& user, const Json::Value& present, int num, int type) {
	if (user.customed_user_data == nullptr) {
		return -1;
	}

	Json::Value& user_presents = user.customed_user_data->json["presents"];
	int id_tmp;
	int present_id = present["id"].asInt();
	if (user_presents.isArray()) {
		for (int i = 0; i < user_presents.size(); i++) {
			loadIntByKeyword("id", user_presents[i], id_tmp);
			if (id_tmp == present_id) {
				switch (type) {
				case presentEdit::PRESENT_ADD:
					num += user_presents[i]["num"].asInt();
					break;
				case presentEdit::PRESENT_CONSUME:
					num = user_presents[i]["num"].asInt() - num;
					if (num < 0) {
						return -1;
					}
					break;
				}
				user_presents[i]["num"] = num;
				user_presents[i]["favor"] = present["favor"].asInt();
				user_presents[i]["name"] = present["name"].asString();
				return num;
			}
		}
	}
	if (type == presentEdit::PRESENT_CONSUME) {
		return -1;
	}
	Json::Value tmp;
	tmp["id"] = present_id;
	tmp["name"] = present["name"].asString();
	tmp["favor"] = present["favor"].asInt();
	tmp["num"] = num;
	user_presents.append(tmp);
	return num;
}

int queryFavor(CQmsg& msg) {
	if (!checkPermission(msg, QUERY_FAVOR_PERMISSION)) {
		return 0;
	}

	pCQuser_t user_tmp = getMsgSender(msg);
	Json::Value& user_data = user_tmp->customed_user_data->json;
	CQJsonMsg reply_msg;

	if (msg.msg_type == messageType::MSG_GROUP) {
		reply_msg.append(CQJmsg::at(user_tmp->id));
	}
	
	if (user_data.isMember("favor")) {
		reply_msg.append(_U("您当前的好感度为: "));
		reply_msg.append(std::to_string(user_data["favor"].asInt()));
	}
	else {
		reply_msg.append(_U("你是？"));
	}
	sendReply(msg, reply_msg.getJson());
	return 1;
}

void register_queryFavor(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = queryFavor;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_MATCH;
	event_tmp.trig_msg.emplace_back(_G("/好感度"));
	event_tmp.trig_msg.emplace_back(_G("#好感度"));
	event_tmp.msg_codetype = CODE_UTF8;
	//Event tag
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = QUERY_FAVOR_PERMISSION;
	event_tmp.tag.name = _G("好感度");
	event_tmp.tag.example = _G("/好感度; #好感度");
	event_tmp.tag.description = _G("查询好感度");

	event_list.push_back(event_tmp);
}

void alpha_basicEvents::setImgFolder(const std::string& path) {
	img_folder = path;
	img_folder_f = "file:///" + img_folder;
}

void alpha_basicEvents::setSauceNAO(const std::string& api_key) {
	sauce_api_key = api_key;
}

void alpha_basicEvents::loadPresents() {
	double sum = 0;
	ava_presents.presents.file_name = PRESENTS_PATH;
	if (!ava_presents.presents.load()) {
		PRINTLOG("WARNING: Cannot load presents list.");
	}

	const Json::Value& json_tmp = ava_presents.presents.json;
	if (!json_tmp.isArray()) {
		return;
	}
	for (int i = 0; i < json_tmp.size(); i++) {
		sum += json_tmp[i]["percentage"].asDouble();
		ava_presents.percent_sum.emplace_back(sum);
	}
}

void alpha_basicEvents::setActiveUsers(jsonFileGroup& active_users){
	active_grp_users = &active_users;
}
