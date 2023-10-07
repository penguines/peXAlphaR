#ifndef ALPHA_BASIC_EVENT_H
#define ALPHA_BASIC_EVENT_H

#include <time.h>
#include <thread>
#include <curl/curl.h>
#include "cq_msg_event.h"
#include "json_file.h"
#include "cq_json_msg.h"
#include "alpha_debug.h"
#include "alpha_permissions.h"
#include "alpha_runtime.h"

#define ALPHA_DAILY_SINGIN_PERMISSION 0
#define ALPHA_SIGN_IMG_1 "sign1.jpg"
#define ALPHA_SIGN_IMG_2 "sign2.jpg"

#define ALPHA_IMGSEARCH_PERMISSION 0

#define PRESENTS_PATH "data/presents.json"
//#define PRESENTS_PATH "F:/peXAlpha_Reborn/peXAlphaR/x64/Release/data/presents.json"
#define DRAW_PRESENTS_PERMISSION	0
#define SHOW_PRESENTS_PERMISSION	0
#define USE_PRESENTS_PERMISSION		0
#define THROW_PRESENTS_PERMISSION	0
#define USE_PRESENTS_REGEX			"^(ËÍ|³Ô)(([1-9][0-9]*)¸ö)?(\\S+)$"
#define THROW_PRESENTS_REGEX		"^\\s*ÈÓ(([1-9][0-9]*)¸ö)?(\\S+)\\s*$"

#define QUERY_FAVOR_PERMISSION		0

enum presentEdit{ PRESENT_SET = 0, PRESENT_ADD = 1, PRESENT_CONSUME = 2 };

namespace alpha_basicEvents {
	struct presentsList {
		JsonFile_t presents;
		std::vector<double> percent_sum;
	};

	void setImgFolder(const std::string& path);
	void setSauceNAO(const std::string& api_key);
	void loadPresents();
	void setActiveUsers(jsonFileGroup& active_users);
}

//Daily sign in
int dailySignIn(CQmsg& msg);
void register_dailySignIn(std::vector<CQMsgEvent>& event_list);

//Reply of at message
int atSB(CQmsg& msg);
int condition_atSB(CQmsg& msg);
void register_atSB(std::vector<CQMsgEvent>& event_list);

//Image search with SauceNAO
void imageSearch(const std::string& img_url, std::string& response);
void imageSearchProc(std::string image_url, std::string reply_id, std::string at_id, int msg_type);
void imgSearchAnsJson(const std::string& resultJson,
					std::string& similarity,
					std::string& imgUrl,
					std::string& src,
					std::string& artist,
					std::string& artist_id,
					std::string& title,
					std::string& srcUrl,
					std::string& imgId);

int imgSearch(CQmsg& msg);
void register_imgSearch(std::vector<CQMsgEvent>& event_list);

//Draw presents
int drawPresents(CQmsg& msg);
void register_drawPresents(std::vector<CQMsgEvent>& event_list);
int multiDrawPresents(CQmsg& msg, int times);
int drawPresents10(CQmsg& msg);
void register_drawPresents10(std::vector<CQMsgEvent>& event_list);

//Show presents
int showPresents(CQmsg& msg);
void register_showPresents(std::vector<CQMsgEvent>& event_list);

//User presents
int usePresents(CQmsg& msg);
void register_usePresents(std::vector<CQMsgEvent>& event_list);

//Throw objects
int throwPresents(CQmsg& msg);
void register_throwPresents(std::vector<CQMsgEvent>& event_list);

//Only edit json without save.
int editUserPresent(CQuser& user, const Json::Value& present, int num, int type = PRESENT_SET);

//Query user favor
int queryFavor(CQmsg& msg);
void register_queryFavor(std::vector<CQMsgEvent>& event_list);

#endif // !ALPHA_BASIC_EVENT_H
