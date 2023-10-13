#include "alpha_cv_event.h"

using namespace alphaCV;
static std::string img_folder_path;
static std::string run_folder_path;
static cv::Mat xibao_bkg;
static std::vector<headData> cascade_heads;

void alpha_cvEvent::setImgFolder(const std::string& folder_path) {
	img_folder_path = folder_path;
}

void alpha_cvEvent::initImages(){
	xibao_bkg = cv::imread(img_folder_path + XIBAO_BKG_FILE, cv::IMREAD_COLOR);
}

void alpha_cvEvent::loadCatcatHeads() {
	cascade_heads.emplace_back();
	cascade_heads.back().load(25, 83, 63, 120, (img_folder_path + "head1.png").c_str(), cv::Point(38, 83), cv::Point(72, 81));
	cascade_heads.emplace_back();
	cascade_heads.back().load(57, 172, 108, 214, (img_folder_path + "head2.png").c_str(), cv::Point(76, 142), cv::Point(143, 142));
	cascade_heads.emplace_back();
	cascade_heads.back().load(151, 409, 319, 547, (img_folder_path + "head3.png").c_str(), cv::Point(212, 390), cv::Point(365, 390));
}

void alpha_cvEvent::setRunFolder(const std::string& folder_path){
	run_folder_path = folder_path;
}

char isASCIIStr(const char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] & 0x80)return 0;
	}
	return 1;
}

void phStyledText(cv::Mat& OutputMat, int textWidth, const char* strBlack, const char* strYellow, const char* fontEN, const char* fontZH) {
	static const int lineWidth = 900;
	static const double yTransRatio = 1 / 16.;
	static const double heightExtendRatio = 0.1;
	static const double widthExtendRatio = 0.05;
	static const double rectHeightRatio = 1.1;
	static const double rectWidthRatio = 1.1;
	static const double rndedRectRatioH = 0.1;
	int textWidth1 = textWidth, textWidth2 = textWidth;
	int maxHeight, imgHeight, rectHeight;
	int imgWidth, rectWidth, contentWidth;
	int midHeight, rectDist;
	int transY1 = 0, transY2 = 0;
	bool isAscii1 = isASCIIStr(strBlack), isAscii2 = isASCIIStr(strYellow);
	cv::Mat textImg;
	cv::Size textSz1, textSz2;
	cv::Point textOrg1, textOrg2, rectOrg;
	std::string font1 = fontEN, font2 = fontEN;

	if (!isAscii1) {
		font1 = fontZH;
	}
	if (!isAscii2) {
		font2 = fontZH;
	}

	textSz1 = getTextSize(strBlack, textWidth1, font1.c_str(), lineWidth);
	textSz2 = getTextSize(strYellow, textWidth2, font2.c_str(), lineWidth);
	if (strcmp(fontZH, _G("微软雅黑")) == 0) {
		if (isAscii1 ^ isAscii2) {
			transY1 = -textSz1.height * yTransRatio * (1 - (int)isAscii1);
			transY2 = -textSz2.height * yTransRatio * (1 - (int)isAscii2);
		}
	}

	maxHeight = (textSz1.height > textSz2.height ? textSz1.height : textSz2.height);
	rectHeight = maxHeight * rectHeightRatio;
	imgHeight = rectHeight * (1 + heightExtendRatio);
	midHeight = imgHeight >> 1;
	rectWidth = textSz2.width * rectWidthRatio;
	rectDist = ((rectWidth - textSz2.width) >> 1);
	contentWidth = (rectWidth << 1) + textSz1.width - textSz2.width;
	imgWidth = contentWidth * (1 + widthExtendRatio);

	textOrg1.x = ((imgWidth - contentWidth) >> 1) + rectDist;
	textOrg1.y = midHeight - (textSz1.height >> 1) + transY1;
	rectOrg.x = textOrg1.x + textSz1.width + rectDist;
	rectOrg.y = midHeight - (rectHeight >> 1);
	textOrg2.x = rectOrg.x + rectDist;
	textOrg2.y = midHeight - (textSz2.height >> 1) + transY2;

	textImg = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
	roundedRect(textImg, rectOrg, rectWidth, rectHeight, rectHeight * rndedRectRatioH, cv::Scalar(0, 154, 254));
	putTextZH(textImg, strBlack, textOrg1, cv::Scalar(255, 255, 255), textWidth1, font1.c_str(), lineWidth);
	putTextZH(textImg, strYellow, textOrg2, cv::Scalar(0, 0, 0), textWidth2, font2.c_str(), lineWidth);
	OutputMat = textImg.clone();
}

void xibaoText(cv::InputArray xb_bkg, cv::OutputArray dst, const std::string& text){
	if (text.empty()) {
		return;
	}
	xb_bkg.copyTo(dst);
	cv::Mat& dst_tmp = dst.getMatRef();
	cv::Size sz;
	sz = getTextSize(text.c_str(), XIBAO_LETTER_SIZE, XIBAO_LETTER_FONT);
	
	putTextZH(dst_tmp, text.c_str(), cv::Point((dst_tmp.cols - sz.width) / 2, (dst_tmp.rows - sz.height) / 2), cv::Scalar(0x0, 0x0, 0xff), XIBAO_LETTER_SIZE, XIBAO_LETTER_FONT);
}

void phStyledTextGen(const CQmsg& msg){
	std::regex reg(PH_STYLEDTEXT_TRIG_REGEX);
	std::smatch reg_result;
	std::string msg_text = msg.uniText();
	std::regex_search(msg_text, reg_result, reg);
	std::string str_black, str_yellow;
	str_black = reg_result[1].str();
	str_yellow = reg_result[2].str();

	cv::Mat ph_img;
	phStyledText(ph_img, PH_STYLEDTEXT_DEFAULT_WIDTH, str_black.c_str(), str_yellow.c_str(), "Arial", _G("微软雅黑"));
	
	CQJsonMsg send_msg;
	send_msg += CQJmsg::image(ph_img);
	sendReply(msg, send_msg.getJson());
}

void xibaoTextGen(const CQmsg& msg){
	std::regex reg(XIBAO_TRIG_REGEX);
	std::smatch reg_result;
	std::string msg_text = msg.uniText();
	std::regex_search(msg_text, reg_result, reg);
	std::string text;
	text = reg_result[1].str();

	cv::Mat img;
	xibaoText(xibao_bkg, img, text);

	CQJsonMsg send_msg(CQJmsg::image(img));
	sendReply(msg, send_msg.getJson());
}

void changeHeadProc(std::string group_id, std::string img_filename, std::string cascade_face, std::string cascade_eyes, std::vector<int> head_index, int head_num){
	std::string img_path;
	size_t img_size;
	getImage(img_filename, img_path, img_size);
	img_size >>= 10;
	PRINTLOG("[接头霸王] 图像大小: %d kb", img_size);
	if (img_size > 2048) {
		sendGroupMsg(group_id, _G("图片过大，请尝试缩小图片(Max 2MB)。"), 0);
		return;
	}
	cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);
	int weld_num = detectChangeHeads(img, cascade_heads, cascade_face, cascade_eyes, head_index.data(), true, head_num);
	PRINTLOG("[接头霸王] 发现%d个焊点!\n", weld_num);

	if (weld_num) {
		CQJsonMsg reply_msg(_U("发现了"));
		reply_msg.append(std::to_string(weld_num)).append(_U("个焊接点！\n"));
		std::string img_temp_path = img_folder_path + img_filename + ".jpg";
		cv::imwrite(img_temp_path, img);
		reply_msg.append(CQJmsg::image("file:///" + img_temp_path));
		sendGroupMsg(group_id, reply_msg.getJson());
		remove(img_temp_path.c_str());
		return;
	}
	sendGroupMsg(group_id, _G("未发现焊接点！"), 0);
}

int phSTEvent(CQmsg& msg){
	if (!checkPermission(msg, PH_STYLEDTEXT_PERMISSION)) {
		return 0;
	}

	std::thread thread_tmp(phStyledTextGen, std::cref(msg));
	thread_tmp.detach();
	return 1;
}

void register_phSTEvent(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = phSTEvent;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = _G(PH_STYLEDTEXT_TRIG_REGEX);
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = PH_STYLEDTEXT_PERMISSION;
	event_tmp.tag.name = _G("神秘字符");
	event_tmp.tag.example = _G("/ph [字符串1] [字符串2]");
	event_tmp.tag.description = _G("基于给定字符生成一张神秘图片。");

	event_list.push_back(event_tmp);
}

int xibaoEvent(CQmsg& msg){
	if (!checkPermission(msg, XIBAO_PERMISSION)) {
		return 0;
	}

	std::thread thread_tmp(xibaoTextGen, std::cref(msg));
	thread_tmp.detach();
	return 1;
}

void register_xibaoEvent(std::vector<CQMsgEvent>& event_list){
	CQMsgEvent event_tmp;
	event_tmp.event_func = xibaoEvent;
	event_tmp.event_type = EVENT_ALL;
	event_tmp.trig_type = MSG_REGEX;
	event_tmp.trig_regex = _G(XIBAO_TRIG_REGEX);
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = XIBAO_PERMISSION;
	event_tmp.tag.name = _G("喜报！");
	event_tmp.tag.example = _G("喜报 [字符串1]");
	event_tmp.tag.description = _G("喜报！喜报！");

	event_list.push_back(event_tmp);
}

int changeHead(CQmsg& msg){
	if (!checkPermission(msg, CHANGE_HEAD_PERMISSION)) {
		return 0;
	}

	const Json::Value& msg_image = getJsonByKeyword(msg.content, "type", "image");
	if (msg_image.empty()) {
		return 0;
	}
	std::string img_file(msg_image["data"]["file"].asString());
	std::string cascade_face = run_folder_path + CASCADE_FACE_PATH;
	std::string cascade_eyes = run_folder_path + CASCADE_EYES_PATH;
	std::vector<std::string> msg_split;
	std::string msg_text = msg.uniText();
	int head_num = 1;
	std::vector<int> head_index;

	split(msg_text, " ", msg_split);
	if (msg_split.size() >= 2) {
		head_num = atoi(msg_split[1].c_str());
		for (int i = 2; i < msg_split.size(); i++) {
			head_index.emplace_back(atoi(msg_split[i].c_str()));
		}
		if (head_num > head_index.size()) {
			head_num = head_index.size();
		}
	}
	else {
		head_index.emplace_back(0);
	}
	CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);

	std::thread changehead_thread(changeHeadProc, grp_msg.group->id, img_file, cascade_face, cascade_eyes, head_index, head_num);
	changehead_thread.detach();
	return 1;
}

void register_changeHead(std::vector<CQMsgEvent>& event_list) {
	CQMsgEvent event_tmp;
	event_tmp.event_func = changeHead;
	event_tmp.event_type = EVENT_GROUP;
	event_tmp.trig_type = MSG_CONTAIN;
	event_tmp.trig_msg.emplace_back(_G(CHANGE_HEAD_MSG));
	event_tmp.msg_codetype = CODE_UTF8;
	event_tmp.tag.index = 0;
	event_tmp.tag.permission = CHANGE_HEAD_PERMISSION;
	event_tmp.tag.name = _G("接头");
	event_tmp.tag.example = _G("接头[图片]");
	event_tmp.tag.description = _G("接头霸王凯露.jpg");

	event_list.push_back(event_tmp);
}