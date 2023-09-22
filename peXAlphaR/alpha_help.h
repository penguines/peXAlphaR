#ifndef ALPHA_HELP_H
#define ALPHA_HELP_H

#include "cq_api_post.h"
#include "msg_event_register.h"
#include "alpha_permissions.h"

#define GET_HELP_PERMISSION_LEVEL	0
#define SEARCH_HELP_TRIG_REGEX		"^(/help|/°ïÖú|#°ïÖú)\\s*(\\S+)\\s*$"

namespace alpha_help {
	void setMsgEventRegister(msgEventRegister& event_reg);
	void setOwnerID(std::string& id);
};

int getHelp(CQmsg& msg);
void register_getHelp(std::vector<CQMsgEvent>& event_list);

int searchHelp(CQmsg& msg);
void register_searchHelp(std::vector<CQMsgEvent>& event_list);

#endif // !ALPHA_HELP_H
