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
    std::string ip, custom_description;
    uint16_t port = 0;
    ip = reg_result[1].str();
    port = atoi(reg_result[3].str().c_str());
    if (port == 0) {
        port = 25565;
    }
    custom_description = reg_result[5];

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string reply_msg(_U("成功添加服务器，ip: "));
    reply_msg.append(ip).append(":").append(std::to_string(port));
    if (!custom_description.empty()) {
        reply_msg.append("\n").append(custom_description);
    }

    Json::Value server_tmp;
    server_tmp["ip"] = ip;
    server_tmp["port"] = port;
    server_tmp["description"] = custom_description;

    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (!servers.empty()) {
        server_tmp["index"] = servers["servers"].size() + 1;
        servers["servers"].append(server_tmp);
        MCServerList.save();
        sendGroupMsg(grp_msg.group->id, reply_msg, 1);
        return 1;
    }
    Json::Value json_tmp;
    json_tmp["group_id"] = grp_id;
    server_tmp["index"] = 1;
    json_tmp["servers"].append(server_tmp);
    server_list.append(json_tmp);
    MCServerList.save();
    sendGroupMsg(grp_msg.group->id, reply_msg, 1);
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
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
        return 0;
    }
    Json::Value& server_datas = servers["servers"];
    if (!server_datas.isArray() || remove_index > server_datas.size()) {
        sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
        return 0;
    }
    for (int j = 0; j < server_datas.size(); j++) {
        if (server_datas[j]["index"].asInt() == remove_index) {
            Json::Value del;
            server_datas.removeIndex(j, &del);
            break;
        }
    }
    for (int j = 0; j < server_datas.size(); j++) {
        server_datas[j]["index"] = j + 1;
    }
    sendGroupMsg(grp_msg.group->id, _G("成功删除服务器。"), 0);
    MCServerList.save();
    return 1;
}

int editMCServer(CQmsg& msg){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    std::regex reg(_U(EDIT_MCSERVER_REGEX));
    std::smatch reg_result;
    std::regex_search(msg.text(), reg_result, reg);
    int edit_index = atoi(reg_result[1].str().c_str());
    std::string ip(reg_result[3].str()), port(reg_result[5].str());
    std::string description(reg_result[7].str());
    std::string reply_str;

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
        return 0;
    }
    Json::Value& server_datas = servers["servers"];
    if (!server_datas.isArray() || edit_index > server_datas.size()) {
        sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
        return 0;
    }
    Json::Value& edit_server = getJsonByKeyword(server_datas, "index", edit_index);
    if(edit_server.empty()) {
        sendGroupMsg(grp_msg.group->id, _G("未找到指定服务器。"), 0);
        return 0;
    }
    if (!ip.empty()) {
        edit_server["ip"] = ip;
        reply_str.append("ip: ").append(ip).append("\n");
    }
    if (!port.empty()) {
        edit_server["port"] = atoi(port.c_str());
        reply_str.append(_U("端口: ")).append(port).append("\n");
    }
    if (!description.empty()) {
        edit_server["description"] = description;
        reply_str.append(_U("服务器描述: ")).append(ip).append("\n");
    }
    if (!reply_str.empty()) {
        MCServerList.save();
        reply_str.append(_U("已成功修改服务器")).append(reg_result[1].str()).append(_U("。"));
        sendGroupMsg(grp_msg.group->id, reply_str, 1);
        return 1;
    }
    sendGroupMsg(grp_msg.group->id, _G("Error: 未提供修改属性。"), 0);
    return 0;
}

int showMCServerInfo(CQmsg& msg){
    const Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    const Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_msg.group->id, _U("本群目前无服务器！"), 1);
        return 0;
    }
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
            reply_msg.append(_U("服务器序号: ")).append(std::to_string((*iter)["index"].asInt())).append("\n");
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
                    description_str = _U("【数据删除】");
                }
                players = server_info["players"];
                reply_msg.append(_U("描述: ")).append(description_str).append("\n");
                reply_msg.append(_U("版本: ")).append(server_info["version"]["name"].asString()).append("\n");
                reply_msg.append(_U("当前在线人数: ")).append(std::to_string(players["online"].asInt()))\
                    .append("/").append(std::to_string(players["max"].asInt())).append("\n");
                if (players.isMember("sample")) {
                    reply_msg.append(_U("当前在线玩家: \n"));
                    players_list = players["sample"];
                    for (auto pl = players_list.begin(); pl != players_list.end(); pl++) {
                        reply_msg.append((*pl)["name"].asString()).append(", ");
                    }
                    reply_msg.erase(reply_msg.length() - 2, 2);
                    reply_msg.append("\n");
                }
                break;
            case SOCKET_CREATE_ERROR:
                reply_msg.append(_U("无法连接：SOCKET创建失败。\n"));
                break;
            case SOCKET_ADDRESS_ERROR:
                reply_msg.append(_U("无法连接：无效的服务器ip。\n"));
                break;
            case SOCKET_CONNECT_ERROR:
                reply_msg.append(_U("无法连接：服务器连接失败。\n"));
                break;
            default:
                reply_msg.append(_U("无法连接：怪耶~\n"));
                break;
            }
            description_str = (*iter)["description"].asString();
            if (!description_str.empty()) {
                reply_msg.append(description_str).append("\n");
            }
            reply_msg.append("--------------------------\n");
        }
        sendGroupMsg(grp_msg.group->id, reply_msg, 1);
        return 1;
    }
    sendGroupMsg(grp_msg.group->id, _U("本群目前无服务器！"), 1);
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
    reg.assign(_U(EDIT_MCSERVER_REGEX));
    if (std::regex_search(msg_text, reg)) {
        if (checkPermission(msg, EDIT_MCSERVER_PERMISSION)) {
            return editMCServer(msg);
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
    event_tmp.tag.example = _G("\n/server show\n/server add ip=[ip地址或域名，不包含端口] (port=[端口号，缺省为25565]) (description=[服务器描述])\n/server remove [服务器序号]");
    event_tmp.tag.example.append(_G("/server edit (ip=[ip地址或域名，不包含端口]) (port=[端口号]) (description=[服务器描述])"));
    event_tmp.tag.description = _G("显示/添加/删除/编辑mc服务器。");

    event_list.push_back(event_tmp);
}

void alpha_mcEvents::loadMCServers(const std::string& folder_path){
    MCServerList.file_path = folder_path;
    MCServerList.file_name = MC_SERVER_PATH;
    MCServerList.load();
}
