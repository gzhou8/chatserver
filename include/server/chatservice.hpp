#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <mutex>
#include <muduo/net/TcpServer.h>
#include <unordered_map>
#include <functional>
using namespace std;
using namespace muduo::net;
using namespace muduo;

#include "json.hpp"
using json = nlohmann::json;

#include "redis.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"


using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js, Timestamp)>;


class ChatService
{
public:

    static ChatService* instance();

    void login(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void reg(const TcpConnectionPtr &conn,json &js, Timestamp time);

    MsgHandler getHandler(int msgid);

    void clientCloseException(const TcpConnectionPtr &conn);

    void oneChat(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void addFriend(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void createGroup(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void addGroup(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void groupChat(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void loginOut(const TcpConnectionPtr &conn,json &js, Timestamp time);

    void reset();

    void handleRedisSubscribeMessage(int userid, string msg);
private:
    ChatService();

    unordered_map<int,MsgHandler> _msg_HandlerMap;

    unordered_map<int,TcpConnectionPtr> _userConnMap;

    mutex _connMutex;

    //数据操作类对象
    UserModel _userModel;

    OfflineMsgModel _offlineMsgModel;

    FriendModel _friendModel;

    GroupModel _groupModel;

    Redis _redis;
};


#endif