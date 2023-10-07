#include "alpha_cv_event.h"

using namespace alphaCV;
static std::string img_folder_path;
static cv::Mat xibao_bkg;

void alpha_cvEvent::setImgFolder(const std::string& folder_path) {
	img_folder_path = folder_path;
}

void alpha_cvEvent::initImages(){
	xibao_bkg = cv::imread(img_folder_path + XIBAO_BKG_FILE, cv::IMREAD_COLOR);
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
	if (strcmp(fontZH, _G("Î¢ÈíÑÅºÚ")) == 0) {
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
	phStyledText(ph_img, PH_STYLEDTEXT_DEFAULT_WIDTH, str_black.c_str(), str_yellow.c_str(), "Arial", _G("Î¢ÈíÑÅºÚ"));
	
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

	CQJsonMsg send_msg;
	send_msg += CQJmsg::image(img);
	sendReply(msg, send_msg.getJson());
}

int phSTEvent(CQmsg& msg){
	if (!checkPermission(msg, PH_STYLEDTEXT_PERMISSION)) {
		return 0;
	}

	std::thread thread_tmp(phStyledTextGen, std::cref(msg));
	thread_tmp.join();
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
	event_tmp.tag.name = _G("ÉñÃØ×Ö·û");
	event_tmp.tag.example = _G("/ph [×Ö·û´®1] [×Ö·û´®2]");
	event_tmp.tag.description = _G("»ùÓÚ¸ø¶¨×Ö·ûÉú³ÉÒ»ÕÅÉñÃØÍ¼Æ¬¡£");

	event_list.push_back(event_tmp);
}

int xibaoEvent(CQmsg& msg){
	if (!checkPermission(msg, XIBAO_PERMISSION)) {
		return 0;
	}

	std::thread thread_tmp(xibaoTextGen, std::cref(msg));
	thread_tmp.join();
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
	event_tmp.tag.name = _G("Ï²±¨£¡");
	event_tmp.tag.example = _G("Ï²±¨ [×Ö·û´®1]");
	event_tmp.tag.description = _G("Ï²±¨£¡Ï²±¨£¡");

	event_list.push_back(event_tmp);
}

