#ifndef ALPHA_CV_EVENT_H
#define ALPHA_CV_EVENT_H

#include <thread>
#include "cq_json_msg.h"
#include "cq_api_post.h"
#include "cq_event.h"
#include "alpha_permissions.h"
#include "alpha_cv.h"

#include "realesrgan_exe.h"

#define PH_STYLEDTEXT_PERMISSION	0
#define PH_STYLEDTEXT_TRIG_REGEX	"^/ph\\s*(\\S+)\\s*(\\S+)$"
#define PH_STYLEDTEXT_DEFAULT_WIDTH	100

#define XIBAO_PERMISSION	0
#define XIBAO_LETTER_COLOR	0xff0000
#define XIBAO_LETTER_SIZE	35
#define XIBAO_LETTER_FONT	"梦源宋体 CN W20"
#define XIBAO_TRIG_REGEX	"^喜报\\s*(\\S+)$"
#define XIBAO_BKG_FILE		"xibao.png"

namespace alpha_cvEvent {
	void setImgFolder(const std::string& folder_path);
	void initImages();
}

char isASCIIStr(const char* str);

void phStyledText(cv::Mat& OutputMat,
				int textWidth,
				const char* strBlack,
				const char* strYellow,
				const char* fontEN,
				const char* fontZH);
void xibaoText(cv::InputArray xb_bkg, cv::OutputArray dst, const std::string& text);

//多线程用std::cref()传入参数
void phStyledTextGen(const CQmsg& msg);
void xibaoTextGen(const CQmsg& msg);

int phSTEvent(CQmsg& msg);
void register_phSTEvent(std::vector<CQMsgEvent>& event_list);
int xibaoEvent(CQmsg& msg);
void register_xibaoEvent(std::vector<CQMsgEvent>& event_list);

#endif // !ALPHA_CV_EVENT_H
