#ifndef DBPOOL_H
#define DBPOOL_H

#include<string>
#include<queue>
using namespace std;
#include"db.h"
#include<mutex>
#include<thread>
#include<functional>
#include<atomic>
#include<condition_variable>
/*
实现连接池模块
*/

class ConnectionPool
{
public:
	static ConnectionPool* getConnectionPool();
	// 给外部提供接口，提供一个可用的空闲连接
	shared_ptr<MySQL> getConnection();
	~ConnectionPool();
private:
	void operator=(const ConnectionPool&) = delete;
	ConnectionPool(const ConnectionPool&) = delete;
	ConnectionPool(); // 单例 构造函数私有化
	
	// 运行在独立的线程中，专门负责生产新连接
	void produceConnectionTask();

	// 扫描多余的空闲连接，超过maxIndleTime
	void scannerConnectionTask();

	// // 从配置文件加载配置项
	// bool loadConfigFile();

	// string _ip; // mysql ip地址
	// unsigned short _port; // mysql 端口号
	// string _username; // mysql 用户名
	// string _password; // mysql 密码
	// string _dbname; // 数据库名称
	int _initSize = 10;	 // mysql 初始连接量
	int _maxSize = 1024;	 // mysql的最大连接量
	int _maxIdletime = 60;// 最大空闲时间
	int _connectionTimeout = 100; // 超时时间

	queue<MySQL*> _connectionQue; // 存储mysql链接的队列
	mutex _queueMutex; // 维护连接队列线程安全的互斥锁
	condition_variable _cv; // 队列条件变量
	atomic_int _connectionCnt; // 记录连接所创建的connection的总量
	//thread produce;
	//thread scanner;
	bool isRun = false;// 判断是否还在运行
};
#endif