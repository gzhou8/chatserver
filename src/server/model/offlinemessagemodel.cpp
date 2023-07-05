#include "offlinemessagemodel.hpp"
#include "dbpool.h"

void OfflineMsgModel::insert(int userid, string msg)
{
    char sql[1024];
    sprintf(sql, "insert into OfflineMessage values(%d,'%s')", userid, msg.c_str());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    sp->update(sql);

    // MySQL mysql;
    // if (mysql.connect())
    // {
    //     mysql.update(sql);
    // }
}

void OfflineMsgModel::remove(int userid)
{
    char sql[1024];
    sprintf(sql, "delete from OfflineMessage where userid=%d", userid);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    sp->update(sql);

    // MySQL mysql;
    // if (mysql.connect())
    // {
    //     mysql.update(sql);
    // }
}

vector<string> OfflineMsgModel::query(int userid)
{
    // 组装sql语句
    char sql[1024];
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    vector<string> vec;

    MySQL mysql;

    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(row[0]);
        }
        mysql_free_result(res);
        return vec;
    }

    return vec;
}
