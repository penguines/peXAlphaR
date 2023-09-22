#ifndef AT_REPLY_H
#define AT_REPLY_H

#include <string>
#include "alpha_runtime.h"
#include "cq_api_post.h"
#include "cq_msg_event.h"
#include "cq_json_msg.h"

#define GLOBAL_GROUP_U64ID	0
#define AT_REPLY_PERMISSION	0
#define MATCH_TYPE_MASK		0x0010
#define MATCH_LCASE_MASK	0x0001

#define ATREPLY_RELOAD_PERMISSION	OWNER_PERMISSION_LEVEL

const std::string AT_REPLIES_PATH = "data/at_replies.json";

namespace alpha_atReply {
	void loadRepliesFile(const std::string& folder_path);
}

int atReply(CQmsg& msg);
int isAtBot(CQmsg& msg);
void register_atReply(std::vector<CQMsgEvent>& event_list);

int atReplyReload(CQmsg& msg);
void register_atReplyReload(std::vector<CQMsgEvent>& event_list);


#endif // !AT_REPLY_H

