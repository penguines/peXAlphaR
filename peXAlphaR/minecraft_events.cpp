#include "minecraft_events.h"

/*
[
    {
        "group_id": uint_64,
        "servers": [{"ip": "ip or realm name", "port": uint_16, "index": }, {}, ...]
    },
    {

    },...
]
*/
JsonFile_t MCServerList;

int addMCServer(CQmsg& msg){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    std::regex reg(_U(ADD_MCSERVER_REGEX));
    std::smatch reg_result;
    std::regex_search(msg.text(), reg_result, reg);
    std::string ip;
    uint16_t port = 0;
    ip = reg_result[1].str();
    if (reg_result.size() == 4) {
        port = atoi(reg_result[3].str().c_str());
    }
    if (port == 0) {
        port = 25565;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string reply_msg(_G("成功添加服务器，ip: "));
    reply_msg.append(ip).append(":").append(std::to_string(port));
    
    Json::Value server_tmp;
    server_tmp["ip"] = ip;
    server_tmp["port"] = port;
    for (int i = 0; i < server_list.size(); i++) {
        Json::Value& servers = server_list[i];
        uint64_t gid_tmp;
        loadUInt64ByKeyword("group_id", servers, gid_tmp);
        if (gid_tmp == grp_id) {
            server_tmp["index"] = servers["servers"].size() + 1;
            servers["servers"].append(server_tmp);
            MCServerList.save();
            sendGroupMsg(grp_msg.group->id, reply_msg, 0);
            return 1;
        }
    }
    Json::Value json_tmp;
    json_tmp["group_id"] = grp_id;
    server_tmp["index"] = 1;
    json_tmp["servers"].append(server_tmp);
    server_list.append(json_tmp);
    MCServerList.save();
    sendGroupMsg(grp_msg.group->id, reply_msg, 0);
    return 1;
}

int removeMCServer(CQmsg& msg){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    std::regex reg(_U(REMOVE_MCSERVER_REGEX));
    std::smatch reg_result;
    std::regex_search(msg.text(), reg_result, reg);
    int remove_index = atoi(reg_result[1].str().c_str());
    
    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    for (int i = 0; i < server_list.size(); i++) {
        Json::Value& servers = server_list[i];
        uint64_t gid_tmp;
        loadUInt64ByKeyword("group_id", servers, gid_tmp);
        if (gid_tmp == grp_id) {
            Json::Value& server_datas = servers["servers"];
            if (!server_datas.isArray() || remove_index > server_datas.size()) {
                sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
                return 0;
            }
            for (int j = 0; j < server_datas.size(); j++) {
                if (server_datas[j]["index"].asInt() == remove_index) {
                    Json::Value del;
                    server_datas.removeIndex(j, &del);
                }
            }
            for (int j = 0; j < server_datas.size(); j++) {
                server_datas[j]["index"] = j + 1;
            }
            sendGroupMsg(grp_msg.group->id, _G("成功删除服务器。"), 0);
            MCServerList.save();
            return 1;
        }
    }
    sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
    return 0;
}

int showMCServerInfo(CQmsg& msg){
    const Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    for (int i = 0; i < server_list.size(); i++) {
        const Json::Value& servers = server_list[i];
        uint64_t gid_tmp;
        loadUInt64ByKeyword("group_id", servers, gid_tmp);
        if (gid_tmp == grp_id) {
            const Json::Value& servers_data = servers["servers"];
            if (servers_data.isArray() && servers_data.size() > 0) {
                std::string reply_msg;
                std::string ip;
                uint16_t port;
                for (auto iter = servers_data.begin(); iter != servers_data.end(); iter++) {
                    Json::Value server_info;
                    Json::Value description_json, players, players_list;
                    std::string description_str;
                    ip = (*iter)["ip"].asString();
                    port = (*iter)["port"].asInt();
                    int state_ret = getServerInfo(ip, port, server_info);
                    reply_msg.append(_G("服务器序号: ")).append(std::to_string((*iter)["index"].asInt())).append("\n");
                    reply_msg.append("ip: ").append(ip).append(":").append(std::to_string(port)).append("\n");
                    if (server_info.isMember("description")) {
                        description_json = server_info["description"];
                    }
                    switch (state_ret)
                    {
                    case 0:
                        if (description_json.isString()) {
                            description_str = description_json.asString();
                        }
                        else if (description_json.isMember("text")) {
                            description_str = description_json["text"].asString();
                        }
                        else if (description_json.isMember("translate")) {
                            description_str = description_json["translate"].asString();
                        }
                        else {
                            description_str = _G("【数据删除】");
                        }
                        players = server_info["players"];
                        reply_msg.append(_G("描述: ")).append(description_str).append("\n");
                        reply_msg.append(_G("版本: ")).append(server_info["version"]["name"].asString()).append("\n");
                        reply_msg.append(_G("当前在线人数: ")).append(std::to_string(players["online"].asInt()))\
                                 .append("/").append(std::to_string(players["max"].asInt())).append("\n");
                        if (players.isMember("sample")) {
                            reply_msg.append(_G("当前在线玩家: \n"));
                            players_list = players["sample"];
                            for (auto pl = players_list.begin(); pl != players_list.end(); pl++) {
                                reply_msg.append((*pl)["name"].asString()).append(", ");
                            }
                            reply_msg.erase(reply_msg.length() - 2, 2);
                            reply_msg.append("\n");
                        }
                        break;
                    case SOCKET_CREATE_ERROR:
                        reply_msg.append(_G("无法连接：SOCKET创建失败。\n"));
                        break;
                    case SOCKET_ADDRESS_ERROR:
                        reply_msg.append(_G("无法连接：无效的服务器ip。\n"));
                        break;
                    case SOCKET_CONNECT_ERROR:
                        reply_msg.append(_G("无法连接：服务器连接失败。\n"));
                        break;
                    default:
                        reply_msg.append(_G("无法连接：怪耶~\n"));
                        break;
                    }
                    reply_msg.append("-----------------------------\n");
                }
                sendGroupMsg(grp_msg.group->id, reply_msg, 0);
                return 1;
            }
            sendGroupMsg(grp_msg.group->id, _G("本群目前无服务器！"), 0);
            return 0;
        }
    }
    sendGroupMsg(grp_msg.group->id, _G("本群目前无服务器！"), 0);
    return 0;
}

int MCServerEvents(CQmsg& msg){
    
    const std::string& msg_text = msg.text();

    std::regex reg(_U(SHOW_MCSERVER_REGEX));
    if (std::regex_search(msg_text, reg)) {
        return showMCServerInfo(msg);
    }
    reg.assign(_U(ADD_MCSERVER_REGEX));
    if (std::regex_search(msg_text, reg)) {
        if (checkPermission(msg, ADD_MCSERVER_PERMISSION)) {
            return addMCServer(msg);
        }
        else {
            sendReply(msg, _G("指令权限不足。"), 0);
            return 0;
        }
    }
    reg.assign(_U(REMOVE_MCSERVER_REGEX));
    if (std::regex_search(msg_text, reg)) {
        if (checkPermission(msg, REMOVE_MCSERVER_PERMISSION)) {
            return removeMCServer(msg);
        }
        else {
            sendReply(msg, _G("指令权限不足。"), 0);
            return 0;
        }
    }
    return 0;
}

void register_MCServerEvents(std::vector<CQMsgEvent>& event_list){
    CQMsgEvent event_tmp;
    event_tmp.event_func = MCServerEvents;
    //EVENT_GROUP set to EVENT_ALL, 2022-07-30
    event_tmp.event_type = EVENT_GROUP;
    event_tmp.trig_type = MSG_CONTAIN;
    event_tmp.trig_msg.emplace_back(_G("/server"));
    event_tmp.msg_codetype = CODE_UTF8;
    //Event tag
    event_tmp.tag.index = 0;
    event_tmp.tag.permission = SHOW_MCSERVER_PERMISSION;
    event_tmp.tag.name = _G("/server");
    event_tmp.tag.example = _G("/server show\n/server add ip = [ip地址或域名，不包含端口] (port = [端口号，缺省为25565])");
    event_tmp.tag.description = _G("显示/添加/删除mc服务器。");

    event_list.push_back(event_tmp);
}

void alpha_mcEvents::loadMCServers(const std::string& folder_path){
    MCServerList.file_path = folder_path;
    MCServerList.file_name = MC_SERVER_PATH;
    MCServerList.load();
}
