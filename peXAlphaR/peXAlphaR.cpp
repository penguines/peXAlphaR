#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <time.h>
#include <json/json.h>
#include <mutex>
#include "json_file.h"
#include "cq_connection.h"
#include "cq_api_post.h"
#include "cq_msg.h"
#include "cq_event.h"
#include "msg_event_register.h"
#include "time_event_register.h"
#include "func_repeat.h"
#include "at_reply.h"
#include "alpha_debug.h"
#include "alpha_inits.h"
#include "alpha_runtime.h"
#include "alpha_permissions.h"
#include "alpha_help.h"
#include "alpha_time_events.h"
#include "alpha_basic_event.h"
#include "alpha_cv_event.h"
#include "alpha_time.h"
#include "minecraft_server.h"

std::string RUN_PATH;

std::mutex			mtx;
timeEventRegister	time_events_reg;
msgEventRegister	events_reg;
CQGroupList			groups_data;
CQGrpMbrDataList	groups_members_data;
CQUserDataList		users_data;
JsonFile_t			bot_config_json;
alphaConfig			bot_config;
jsonFileGroup		active_group_members;

int					local_UTC				= 8;
//as milliseconds
int					time_step				= 1000;

void timeEventLoop();
void messageProcess(Json::Value& message);

void initialize();
void setTimeEvents(timeEventRegister& event_reg);
void setEvents(msgEventRegister& event_reg);

int main() {
	std::string recv_str;

	Json::Reader json_reader;
	Json::Value recv_json;
	int msg_event_num = 0, time_event_num = 0;

	DEBUGLOG("Start initializing...");
	initialize();
	DEBUGLOG("Initialize finish.");
	DEBUGLOG("Setting events...");
	setEvents(events_reg);
	setTimeEvents(time_events_reg);
	DEBUGLOG("Registering events...");
	msg_event_num = events_reg.registerEvents();
	time_event_num = time_events_reg.registerEvents();
	DEBUGLOG("%d msg event(s) & %d time event(s) registered successfully.", msg_event_num, time_event_num);
	PRINTLOG("¹þ¼ªÂíÂ·Ó´£¡");

	SOCKET serv_sock;
	createConnection(serv_sock, bot_config.recv_host.c_str(), bot_config.recv_port);
	listen(serv_sock, 10000);

	//time thread
	std::thread timeLoop(timeEventLoop);
	timeLoop.detach();
	//msg
	std::string msg_temp;
	while (1) {
		recvJsonMessage(serv_sock, recv_str);
		//std::cout << recv_str << std::endl;

		if (json_reader.parse(recv_str, recv_json)) {
			loadStringByKeyword("post_type", recv_json, msg_temp);
			if (msg_temp == "message") {
				messageProcess(recv_json);
			}
		}

	}
	
	closeConnection(serv_sock);

	WSACleanup();
}

void timeEventLoop(){
	std::chrono::system_clock::time_point now;
	std::chrono::system_clock::time_point awake;
	time_t cur_gmtime = time(NULL);
	a_time* cur_localtime;
	aTime cur_time;

	//second align
	now = std::chrono::system_clock::from_time_t(cur_gmtime);
	awake = now + std::chrono::seconds(1);
	std::this_thread::sleep_until(awake);
	now = std::chrono::system_clock::now();

	while (1) {
		//get current time
		cur_gmtime = std::chrono::system_clock::to_time_t(now);
		awake = now + std::chrono::milliseconds(time_step);
		cur_localtime = localtime(&cur_gmtime);
		cur_time.setTime(*cur_localtime, local_UTC);

		//Codes here
		time_events_reg.executeEvents(cur_time);
		
		//
		std::this_thread::sleep_until(awake);
		now = awake;
	}
}

void messageProcess(Json::Value& message) {
	std::string str_temp;

	CQPrivateMsg private_msg;
	CQGroupMsg group_msg;
	CQmsg* ptr_general_msg;
	//Lock thread.
	std::lock_guard<std::mutex> lock_msg(mtx);

	loadStringByKeyword("message_type", message, str_temp);
	if (str_temp == "private") {
		private_msg.load(message);
		loadCQmsgUserData(private_msg, users_data);

		std::cout << "[" << utf8ToGB2312(private_msg.sender->nickname) <<  "(" << private_msg.sender->id << ")]> ";
		std::cout << utf8ToGB2312(private_msg.toStyledText()) << std::endl << std::endl;

		ptr_general_msg = &private_msg;
	}
	else if (str_temp == "group") {
		group_msg.load(message);
		loadCQmsgUserData(group_msg, users_data);
		loadCQmsgGroupData(group_msg, groups_data, groups_members_data);

		std::cout << "[" << utf8ToGB2312(group_msg.sender->nickname) << "(" << group_msg.sender->id << ")]@(";
		std::cout << group_msg.group->id << ")> ";
		std::cout << utf8ToGB2312(group_msg.toStyledText()) << std::endl << std::endl;

		if (!isGroupAvailable(group_msg)) {
			checkGroupStart(group_msg);
			return;
		}
		ptr_general_msg = &group_msg;
	}
	else {
		return;
	}

	events_reg.executeEvents(*ptr_general_msg);
}

void initialize() {
	setlocale(LC_ALL, "chs");
	std::wcout.imbue(std::locale(""));
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	curl_global_init(CURL_GLOBAL_ALL);

	RUN_PATH = getRunPath();
	//IMG_FOLDER_PATH = RUN_PATH + IMG_FILES_PATH;
	//config init
	initConfig(RUN_PATH, bot_config_json, bot_config);
	cq_post_api::setPostIP(bot_config.post_host, bot_config.post_port);
	cq_post_api::setGoCQFolder(bot_config.gocq_folder);

	//permission init
	alpha_permission::setBotOwner(bot_config.owner_id);
	alpha_permission::setUserData(users_data);

	//data init
	DEBUGLOG("Initializing groups...");
	initGroupsData(RUN_PATH + GROUP_DATA_PATH, groups_data, groups_members_data);
	alpha_runtime::setGroupDataFolder(RUN_PATH + GROUP_DATA_PATH);
	DEBUGLOG("Initializing users...");
	initUsersData(RUN_PATH + USER_DATA_PATH, users_data);
	alpha_runtime::setUserDataFolder(RUN_PATH + USER_DATA_PATH);
	DEBUGLOG("Initializing active users...");
	initActiveUsers(groups_members_data, users_data, active_group_members);
	//DEBUGLOG("%s", active_group_members[_TEST_GROUPID_][0]->identifier.c_str());

	//others
	alpha_help::setMsgEventRegister(events_reg);
	alpha_help::setOwnerID(bot_config.owner_id);

	alpha_basicEvents::setImgFolder(RUN_PATH + IMG_FILES_PATH);
	alpha_basicEvents::setSauceNAO(bot_config.sauce_apikey);
	alpha_basicEvents::loadPresents();
	alpha_basicEvents::setActiveUsers(active_group_members);

	alpha_cvEvent::setImgFolder(RUN_PATH + IMG_FILES_PATH);
	alpha_cvEvent::initImages();

	alpha_atReply::setImgFolder(RUN_PATH + IMG_FILES_PATH);
	alpha_atReply::loadRepliesFile(RUN_PATH);

	alpha_timeEvents::setImgFolder(RUN_PATH + IMG_FILES_PATH);
	alpha_timeEvents::loadSetTimeMention(RUN_PATH);

	realesrganExe::setRunPath(RUN_PATH);
}

void setTimeEvents(timeEventRegister& event_reg){
	event_reg.append(register_setTimeMention, static_cast<void*>(&groups_data));
}

void setEvents(msgEventRegister& event_reg) {
	event_reg.append(register_repeat);
	event_reg.append(register_setRepeatTimes);
	event_reg.append(register_atReply);
	event_reg.append(register_atReplyReload);
	event_reg.append(register_setOperator);
	event_reg.append(register_delOperator);
	event_reg.append(register_getHelp);
	event_reg.append(register_searchHelp);
	event_reg.append(register_enableGroup);
	event_reg.append(register_disableGroup);
	event_reg.append(register_dailySignIn);
	event_reg.append(register_drawPresents);
	event_reg.append(register_drawPresents10);
	event_reg.append(register_showPresents);
	event_reg.append(register_usePresents);
	event_reg.append(register_throwPresents);
	event_reg.append(register_queryFavor);
	event_reg.append(register_imgSearch);
	event_reg.append(register_phSTEvent);
	event_reg.append(register_xibaoEvent);
	//Only available with valid graphic card.
	//event_reg.append(register_imgEnhance);
}