#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <malloc.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name error or pwd error or relogin"

#define SEARCH_USR_NO "No such people"
#define SEARCH_USR_ONLINE "Online"
#define SEARCH_USR_OFFLINE "Offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOEXIST "usr not exist"


enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,   // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,   // 登录回复    

    ENUM_MSG_TYPE_CANCEL_REQUEST,   // 退出请求
    ENUM_MSG_TYPE_CANCEL_RESPOND,   // 退出请求

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   // 在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   // 在线用户回复

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,   // 在线用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,   // 在线用户回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友回复

    ENUM_MSG_TYPE_MAX = 0x00ffffff
};

struct PDU
{
    uint uiPDULen;      // 总的协议数据单元大小
    uint uiMsgType;     // 消息类型
    char caData[64];
    uint uiMsgLen;      // 实际消息长度
    int caMsg[];        // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
