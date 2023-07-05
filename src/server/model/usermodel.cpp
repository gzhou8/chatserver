#include "usermodel.hpp"
#include "dbpool.h"

#include <string>
using namespace std;

bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024];
    sprintf(sql, "insert into User(name,password,state) values('%s','%s','%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    if (sp->update(sql))
    {
        // 插入成功的用户返回主键id
        user.setId(mysql_insert_id(sp->getConnection()));
        return true;
    }

    return false;
}

User UserModel::query(int id)
{
    // 组装sql语句
    char sql[1024];
    sprintf(sql, "select * from User where id = %d", id);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    MySQL mysql;

    MYSQL_RES *res = sp->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);
            mysql_free_result(res);
            return user;
        }
    }

    return User();
}

bool UserModel::updateState(User user)
{
    char sql[1024];
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    if (sp->update(sql))
    {
        return true;
    }

    return false;
}

void UserModel::resetState()
{
    char sql[1024] = "update User set state = 'offline' where state='online'";

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<MySQL> sp = cp->getConnection();

    sp->update(sql);
}
