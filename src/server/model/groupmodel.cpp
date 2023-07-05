#include "groupmodel.hpp"
#include "dbpool.h"

using namespace std;

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into AllGroup(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    if (sp->update(sql))
    {
        group.setId(mysql_insert_id(sp->getConnection()));
        return true;
    }
    return false;

    // MySQL mysql;
    // if (mysql.connect())
    // {
    //     if (mysql.update(sql))
    //     {
    //         // 插入成功的用户返回主键id
    //         group.setId(mysql_insert_id(mysql.getConnection()));
    //         return true;
    //     }
    // }

    // return false;
}
// 加入群组
void GroupModel::addGroup(int groupid, int userid, string role)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser values(%d, %d,'%s')", groupid, userid, role.c_str());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    sp->update(sql);

    // MySQL mysql;
    // if (mysql.connect())
    // {
    //     mysql.update(sql);
    // }
}
// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    // 查询用户所在的所有组
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup a inner join GroupUser b on b.groupid = a.id where b.userid = %d", userid);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    vector<Group> groupVec;
    // MySQL mysql;

    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groupVec.push_back(group);
        }
        mysql_free_result(res);
    }

    // 查询这些组内成员信息，用来建立群聊通信
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from User a inner join GroupUser b on a.id = b.userid where b.groupid = %d", group.getId());
        MYSQL_RES *res = sp->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setRole(row[2]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}
// 根据groupid查询组内除自己外其他成员id
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 组装sql语句
    char sql[1024];
    sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d", groupid, userid);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    vector<int> idVec;

    MySQL mysql;

    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            idVec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }

    return idVec;
}