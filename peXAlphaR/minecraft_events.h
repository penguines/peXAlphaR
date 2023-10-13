#ifndef MINECRAFT_EVENTS_H
#define MINECRAFT_EVENTS_H

#include "alpha_permissions.h"
#include "minecraft_server.h"
#include "cq_msg_event.h"
#include "cq_json_msg.h"

const std::string MC_SERVER_PATH = "data/mc_servers.json";

#define ADD_MCSERVER_PERMISSION		OP_PERMISSION_LEVEL
#define ADD_MCSERVER_REGEX			"add ip\\s*=\\s*(\\S+)\\s*(port\\s*=\\s*([0-9]+))?\\s*(description\\s*=\\s*(\\S+[\\s\\S]*))?"

#define REMOVE_MCSERVER_PERMISSION	OP_PERMISSION_LEVEL
#define REMOVE_MCSERVER_REGEX		"remove ([1-9][0-9]*)"

#define EDIT_MCSERVER_PERMISSION	OP_PERMISSION_LEVEL
#define EDIT_MCSERVER_REGEX			"edit ([1-9][0-9]*)\\s*(ip\\s*=\\s*(\\S+))?\\s*(port\\s*=\\s*([0-9]+))?\\s*(description\\s*=\\s*(\\S+[\\s\\S]*))?"

#define SHOW_MCSERVER_PERMISSION	NORMAL_PERMISSION_LEVEL
#define SHOW_MCSERVER_REGEX			"(show|list)"
#define SHOW_SERVER_BYINDEX_REGEX	"/server\\s*([1-9][0-9]*)(\\s*|\\s+\\S*)$"
#define SHOW_SERVER_BYDESC_REGEX	"/server\\s*(\\S[\\s\\S]*)"


namespace alpha_mcEvents {
	void loadMCServers(const std::string& folder_path);
}

int addMCServer(CQmsg& msg);

int removeMCServer(CQmsg& msg);

int editMCServer(CQmsg& msg);

int generateServerInfo(const Json::Value& server, std::string& info);
//show all servers' info
int showMCServerInfo(CQmsg& msg);
//show info of a single server by index
int showMCServerInfoByIndex(CQmsg& msg, int index);
//show info of a single server by content of description
int showMCServerInfoByDesc(CQmsg& msg, std::string content);

int MCServerEvents(CQmsg& msg);
void register_MCServerEvents(std::vector<CQMsgEvent>& event_list);

#endif // !MINECRAFT_EVENTS_H
