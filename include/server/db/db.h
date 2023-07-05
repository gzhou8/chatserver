#ifndef DB_H
#define DB_H

#include <iostream>
#include <mysql/mysql.h>
#include <string>
using namespace std;

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 释放数据库连接资源
    ~MySQL();
    // 连接数据库
    bool connect(string ip, unsigned short port, string user, string password, string dbname);
    // 更新操作
    bool update(string sql);
    // 查询操作
    MYSQL_RES* query(string sql);
    //获取连接
    MYSQL* getConnection();

    // 刷新一下链接的时间
	void refreshAliveTime()
	{
		_alivetime = clock();
	}

	clock_t getAliveTime()
	{
		return clock();
	}
private:
    MYSQL *_conn;
    clock_t _alivetime; // 存活时间
};

#endif