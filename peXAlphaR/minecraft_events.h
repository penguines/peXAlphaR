#ifndef MINECRAFT_EVENTS_H
#define MINECRAFT_EVENTS_H

#include "alpha_permissions.h"
#include "minecraft_server.h"
#include "cq_msg_event.h"
#include "cq_json_msg.h"

const std::string MC_SERVER_PATH = "data/mc_servers.json";

#define ADD_MCSERVER_PERMISSION		OP_PERMISSION_LEVEL
#define ADD_MCSERVER_REGEX			"add ip\\s*=\\s*(\\S+)\\s*(port\\s*=\\s*([0-9]+))?"

#define REMOVE_MCSERVER_PERMISSION	OP_PERMISSION_LEVEL
#define REMOVE_MCSERVER_REGEX		"remove ([1-9][0-9]*)"

#define SHOW_MCSERVER_PERMISSION	NORMAL_PERMISSION_LEVEL
#define SHOW_MCSERVER_REGEX			"show"

namespace alpha_mcEvents {
	void loadMCServers(const std::string& folder_path);
}

int addMCServer(CQmsg& msg);

int removeMCServer(CQmsg& msg);

int showMCServerInfo(CQmsg& msg);

int MCServerEvents(CQmsg& msg);
void register_MCServerEvents(std::vector<CQMsgEvent>& event_list);

#endif // !MINECRAFT_EVENTS_H
