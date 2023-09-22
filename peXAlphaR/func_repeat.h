#ifndef FUNC_REPEAT_H
#define FUNC_REPEAT_H

#include "cq_msg.h"
#include "cq_api_post.h"
#include "cq_transcoder.h"
#include "cq_msg_event.h"
#include "alpha_permissions.h"

#define DEFAULT_REPEAT_TIMES			3
#define TRIG_REPEAT_PERMISSION_LEVEL	0
#define SET_REPEAT_PERMISSION_LEVEL		2

int repeatMsg(CQmsg& msg);
void register_repeat(std::vector<CQMsgEvent>& event_list);

int setRepeatTimes(CQmsg& msg);
void register_setRepeatTimes(std::vector<CQMsgEvent>& event_list);

#endif // !FUNC_REPEAT_H
