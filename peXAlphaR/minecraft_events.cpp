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
static const cv::Size MCServerImgSize(700, 150);

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
        reply_str.append(_U("服务器描述: ")).append(description).append("\n");
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

int generateServerInfo(Json::Value& server, std::string& info){
    Json::Value server_info;
    Json::Value description_json, players, players_list;
    std::string description_str;
    std::string ip = server["ip"].asString();
    uint16_t port = server["port"].asInt();
    int state_ret = getServerInfo(ip, port, server_info);
    info.append(_U("服务器序号: ")).append(std::to_string(server["index"].asInt())).append("\n");
    info.append("ip: ").append(ip).append(":").append(std::to_string(port)).append("\n");
    if (server_info.isMember("description")) {
        description_json = server_info["description"];
    }
    switch (state_ret)
    {
    case 0:
        if (description_json.isString()) {
            description_str = description_json.asString();
            server["name"] = description_str;
        }
        else if (description_json.isMember("text")) {
            description_str = description_json["text"].asString();
            server["name"] = description_str;
        }
        else if (description_json.isMember("translate")) {
            description_str = description_json["translate"].asString();
            server["name"] = description_str;
        }
        else {
            description_str = _U("【数据删除】");
        }
        players = server_info["players"];
        info.append(_U("描述: ")).append(description_str).append("\n");
        info.append(_U("版本: ")).append(server_info["version"]["name"].asString()).append("\n");
        info.append(_U("当前在线人数: ")).append(std::to_string(players["online"].asInt()))\
            .append("/").append(std::to_string(players["max"].asInt())).append("\n");
        if (players.isMember("sample")) {
            info.append(_U("当前在线玩家: \n"));
            players_list = players["sample"];
            for (auto pl = players_list.begin(); pl != players_list.end(); pl++) {
                info.append((*pl)["name"].asString()).append(", ");
            }
            info.erase(info.length() - 2, 2);
            info.append("\n");
        }
        break;
    case SOCKET_CREATE_ERROR:
        info.append(_U("无法连接：SOCKET创建失败。\n"));
        break;
    case SOCKET_ADDRESS_ERROR:
        info.append(_U("无法连接：无效的服务器ip。\n"));
        break;
    case SOCKET_CONNECT_ERROR:
        info.append(_U("无法连接：服务器连接失败。\n"));
        break;
    default:
        info.append(_U("无法连接：怪耶~\n"));
        break;
    }
    description_str = server["description"].asString();
    if (!description_str.empty()) {
        info.append(description_str).append("\n");
    }
    info.append("--------------------------\n");
    return 1;
}

int generateServerInfo(Json::Value& server, cv::OutputArray img){
    static const cv::Scalar bkg_black(7, 12, 15);
    static const cv::Scalar letter_grey(128, 128, 128);
    static const cv::Scalar letter_white(255, 255, 255);
    static const cv::Scalar ava_green(0, 255, 0);
    static const cv::Scalar off_red(80, 80, 255);
    static const char letter_font[] = "Minecraft AE";
    static const uint8_t name_size = 25;
    static const uint8_t address_size = 20;
    static const uint8_t state_size = 18;
    static const uint8_t player_size = 15;
    static const cv::Point name_loc(100, 30);
    static const cv::Point state_loc(650, 30);
    static const cv::Point player_loc(650, 120);

    img.create(MCServerImgSize, CV_8UC3);
    cv::Mat& m_dst = img.getMatRef();
    m_dst.setTo(bkg_black);
    cv::Size sz_tmp;

    Json::Value server_info;
    Json::Value description_json, players, players_list;
    std::string description_str, player_num, player_names, version;
    std::string name;
    std::string ip = server["ip"].asString();
    uint16_t port = server["port"].asInt();
    std::string address(ip);
    address.append(":").append(std::to_string(port));
    int state_ret = getServerInfo(ip, port, server_info);
    cv::Mat server_icon;
    if (server_info.isMember("description")) {
        description_json = server_info["description"];
    }
    switch (state_ret){
    case 0:
        if (description_json.isString()) {
            description_str = description_json.asString();
            server["name"] = description_str;
        }
        else if (description_json.isMember("text")) {
            description_str = description_json["text"].asString();
            server["name"] = description_str;
        }
        else if (description_json.isMember("translate")) {
            description_str = description_json["translate"].asString();
            server["name"] = description_str;
        }
        else {
            description_str = _U("【数据删除】");
        }

        players = server_info["players"];
        player_num.append(std::to_string(players["online"].asInt())).append("/").append(std::to_string(players["max"].asInt()));
        if (players.isMember("sample")) {
            int pnum = 0;
            player_names.append(_G("在线玩家: "));
            players_list = players["sample"];
            for (auto pl = players_list.begin(); pl != players_list.end(); pl++) {
                player_names.append(utf8ToGB2312((*pl)["name"].asString())).append(", ");
                pnum++;
                if (pnum > 2) {
                    player_names.append("et al., ");
                    break;
                }
            }
            player_names.erase(player_names.length() - 2, 2);

            sz_tmp = alphaCV::getTextSize(player_names.c_str(), player_size, letter_font);
            alphaCV::putTextZH(m_dst, player_names.c_str(), player_loc - cv::Point(sz_tmp.width + 15, 0), letter_grey, player_size, letter_font);
            sz_tmp = alphaCV::getTextSize("!", player_size, letter_font);
            alphaCV::putTextZH(m_dst, "!", player_loc - cv::Point(sz_tmp.width, 0), ava_green, player_size, letter_font);
        }
        if (server_info.isMember("favicon")) {
            std::string icon_str(server_info["favicon"].asString());
            std::string img_b64(icon_str.length(), 0);
            auto it_b64 = img_b64.begin();
            auto iter = icon_str.cbegin();
            while (iter != icon_str.cend() && *iter != ',') {
                iter++;
            }
            iter++;
            for (; iter != icon_str.cend(); iter++) {
                if (!isspace(*iter)) {
                    *it_b64 = *iter;
                    it_b64++;
                }
            }
            std::vector<uchar> img_data;
            base64Decode(img_b64, img_data);
            server_icon = cv::imdecode(img_data, cv::IMREAD_UNCHANGED);
            if (!server_icon.empty()) {
                mergeImage(server_icon, m_dst, name_loc.x - 84, name_loc.y, 1., MERGE_ADD);
            }
        }

        version = utf8ToGB2312(server_info["version"]["name"].asString());

        sz_tmp = alphaCV::getTextSize("服务器在线", state_size, letter_font);
        alphaCV::putTextZH(m_dst, "服务器在线", state_loc - cv::Point(sz_tmp. width, 0), ava_green, state_size, letter_font);
        sz_tmp = alphaCV::getTextSize(player_num.c_str(), state_size, letter_font);
        alphaCV::putTextZH(m_dst, player_num.c_str(), state_loc - cv::Point(sz_tmp.width, -state_size - 10), letter_grey, state_size, letter_font);
        sz_tmp = alphaCV::getTextSize(version.c_str(), state_size, letter_font);
        alphaCV::putTextZH(m_dst, version.c_str(), state_loc - cv::Point(sz_tmp.width, -2 * state_size - 20), letter_grey, state_size, letter_font);

        break;
    case SOCKET_CREATE_ERROR:
    case SOCKET_ADDRESS_ERROR:
    case SOCKET_CONNECT_ERROR:
    default:
        sz_tmp = alphaCV::getTextSize("服务器离线", state_size, letter_font);
        alphaCV::putTextZH(m_dst, "服务器离线", state_loc - cv::Point(sz_tmp.width, 0), off_red, state_size, letter_font);
        break;
    }
    if (server.isMember("name")) {
        name = utf8ToGB2312(server["name"].asString());
        if (name.empty()) {
            name = _G("Minecraft服务器");
        }
    }
    else {
        name = _G("Minecraft服务器");
    }

    alphaCV::putTextZH(m_dst, name.c_str(), name_loc, letter_white, name_size, letter_font);
    alphaCV::putTextZH(m_dst, address.c_str(), name_loc + cv::Point(0, 64 - address_size), letter_grey, address_size, letter_font);

    return 1;
}

/*
int showMCServerInfo(CQmsg& msg){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string grp_cid_tmp = grp_msg.group->id;
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_msg.group->id, _U("本群目前无服务器！"), 1);
        return 0;
    }
    Json::Value& servers_data = servers["servers"];
    if (servers_data.isArray() && servers_data.size() > 0) {
        std::string reply_msg;
        for (auto iter = servers_data.begin(); iter != servers_data.end(); iter++) {
            generateServerInfo(*iter, reply_msg);
        }
        sendGroupMsg(grp_cid_tmp, reply_msg, 1);
        MCServerList.save();
        return 1;
    }
    sendGroupMsg(grp_cid_tmp, _U("本群目前无服务器！"), 1);
    return 0;
}
*/

int showMCServerInfo(CQmsg& msg) {
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string grp_cid_tmp = grp_msg.group->id;
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_msg.group->id, _U("本群目前无服务器！"), 1);
        return 0;
    }
    Json::Value& servers_data = servers["servers"];
    int loc_idx = 0;
    if (servers_data.isArray() && servers_data.size() > 0) {
        CQJsonMsg reply;
        cv::Mat list_img(cv::Size(MCServerImgSize.width, servers_data.size() * MCServerImgSize.height), CV_8UC3);
        cv::Mat tmp;
        for (auto iter = servers_data.begin(); iter != servers_data.end(); iter++) {
            generateServerInfo(*iter, tmp);
            tmp.copyTo(list_img(cv::Rect(0, loc_idx * MCServerImgSize.height, MCServerImgSize.width, MCServerImgSize.height)));
            loc_idx++;
        }
        MCServerList.save();
        reply.append(CQJmsg::image(list_img));
        sendGroupMsg(grp_cid_tmp, reply.getJson());
        return 1;
    }
    sendGroupMsg(grp_cid_tmp, _U("本群目前无服务器！"), 1);
    return 0;
}

int showMCServerInfoByIndex(CQmsg& msg, int index){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string grp_cid_tmp = grp_msg.group->id;
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_cid_tmp, _U("本群目前无服务器！"), 1);
        return 0;
    }
    Json::Value& server_datas = servers["servers"];
    if (!server_datas.isArray() || index > server_datas.size()) {
        sendGroupMsg(grp_cid_tmp, _G("未找到指定服务器。"), 0);
        return 0;
    }
    Json::Value& show_server = getJsonByKeyword(server_datas, "index", index);
    if (show_server.empty()) {
        sendGroupMsg(grp_cid_tmp, _G("未找到指定服务器。"), 0);
        return 0;
    }
    std::string reply_msg;
    generateServerInfo(show_server, reply_msg);
    MCServerList.save();
    sendGroupMsg(grp_cid_tmp, reply_msg, 1);
    return 1;
}

int showMCServerInfoByDesc(CQmsg& msg, std::string content){
    Json::Value& server_list = MCServerList.json;
    if (!server_list.isArray()) {
        return 0;
    }

    CQGroupMsg& grp_msg = static_cast<CQGroupMsg&>(msg);
    uint64_t& grp_id = grp_msg.group->u64_id;
    std::string grp_cid_tmp = grp_msg.group->id;
    Json::Value& servers = getJsonByKeyword(server_list, "group_id", grp_id);
    if (servers.empty()) {
        sendGroupMsg(grp_cid_tmp, _U("本群目前无服务器！"), 1);
        return 0;
    }
    Json::Value& server_datas = servers["servers"];
    if (!server_datas.isArray()) {
        sendGroupMsg(grp_cid_tmp, _G("未找到指定服务器。"), 0);
        return 0;
    }
    std::string reply_msg;
    toLowerCase(content);
    for (auto iter = server_datas.begin(); iter != server_datas.end(); iter++) {
        std::string desc_tmp = (*iter)["description"].asString();
        toLowerCase(desc_tmp);
        if (desc_tmp.find(content) != std::string::npos) {
            generateServerInfo(*iter, reply_msg);
        }
    }
    if (!reply_msg.empty()) {
        MCServerList.save();
        sendGroupMsg(grp_cid_tmp, reply_msg, 1);
        return 1;
    }
    sendGroupMsg(grp_cid_tmp, _G("未找到指定服务器。"), 0);
    return 0;
}

int MCServerEvents(CQmsg& msg){
    
    const std::string& msg_text = msg.text();

    std::regex reg(_U(SHOW_MCSERVER_REGEX));
    if (std::regex_search(msg_text, reg)) {
        std::thread show_server(showMCServerInfo, std::ref(msg));
        show_server.detach();
        return 1;
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
    std::smatch reg_result;
    reg.assign(_U(SHOW_SERVER_BYINDEX_REGEX));
    if (std::regex_search(msg_text, reg_result, reg)) {
        if (checkPermission(msg, SHOW_MCSERVER_PERMISSION)) {
            std::thread show_server(showMCServerInfoByIndex, std::ref(msg), atoi(reg_result[1].str().c_str()));
            show_server.detach();
            return 1;
        }
        else {
            sendReply(msg, _G("指令权限不足。"), 0);
            return 0;
        }
    }
    reg.assign(_U(SHOW_SERVER_BYDESC_REGEX));
    if (std::regex_search(msg_text, reg_result, reg)) {
        if (checkPermission(msg, SHOW_MCSERVER_PERMISSION)) {
            std::thread show_server(showMCServerInfoByDesc, std::ref(msg), reg_result[1].str());
            show_server.detach();
            return 1;
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
    event_tmp.tag.example = _G("\n/server show|list|[服务器序号]|[服务器描述内容]\n");
    event_tmp.tag.example.append(_G("/server add ip=[ip地址或域名，不包含端口] (port=[端口号，缺省为25565]) (description=[服务器描述])\n/server remove [服务器序号]"));
    event_tmp.tag.example.append(_G("/server edit (ip=[ip地址或域名，不包含端口]) (port=[端口号]) (description=[服务器描述])"));
    event_tmp.tag.description = _G("显示/添加/删除/编辑mc服务器。");

    event_list.push_back(event_tmp);
}

void alpha_mcEvents::loadMCServers(const std::string& folder_path){
    MCServerList.file_path = folder_path;
    MCServerList.file_name = MC_SERVER_PATH;
    MCServerList.load();
}
