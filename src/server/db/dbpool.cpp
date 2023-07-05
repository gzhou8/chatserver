#include "dbpool.h"
#include <muduo/base/Logging.h>

// 数据库配置信息
static string _ip = "127.0.0.1";
static string _user = "root";
static string _password = "gzhou8";
static string _dbname = "chat";

ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}
ConnectionPool::~ConnectionPool()
{
    isRun = true;
    _cv.notify_all();
}

// 单例 构造函数私有化
ConnectionPool::ConnectionPool()
{

    // 创建初始的数量连接
    for (int i = 0; i < _initSize; ++i)
    {
        MySQL *p = new MySQL();
        p->connect(_ip, 3306, _user, _password, _dbname);
        p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
        _connectionQue.push(p);
        _connectionCnt++;
    }
    // 启动一个新的线程，作为连接生产者,绑定有一个成员变量，并且传入this指针才能使用
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();
    //  启动一个新的定时线程，扫描多余的空闲连接，超过maxIndleTime
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

// 扫描多余的空闲连接，超过maxIndleTime
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        if (isRun)
            return;
        // 直接睡_maxIdletime，起来就检测一次
        // this_thread::sleep_for(chrono::seconds(_maxIdletime));
        // 扫描整个队列
        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            if (isRun)
                return;
            // 若是每一个线程都占用着连接，此时扫描线程进来后检测到队列为空，就可以直接退出
            if (_connectionQue.empty())
            {
                break;
            }
            // 队头的时间是待在队列最长的
            MySQL *p = _connectionQue.front();
            if (p->getAliveTime() >= _maxIdletime * 1000) // 60s 的话太长了，一般来说不会调用这里pop掉，6s的话这里会进行删除
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p; // 调用~Connection 释放连接
            }
            else
            {
                break; // 队头没有超过超时时间，那么没必要看了
            }
        }
    }
}

// 运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
    // 生产连接需要注意不能超过最大的量
    for (;;)
    {
        if (isRun)
            return;
        unique_lock<mutex> lock(_queueMutex); // 由于wait要释放锁，所以用unique_lock
        while (!isRun && !_connectionQue.empty())
        {
            if (isRun)
                return;
            _cv.wait(lock); // 等待队列变空,此时不需要生产
        }
        if (isRun)
            return; // 不能访问到任何主线程的容器。
        // 走到这里，说明需要生产者生产连接

        // 若常见的连接已经比最大的创建数都多了，就不再创建了，让他们等着其他连接用完，这里补充处理
        if (isRun && _connectionCnt < _maxSize)
        {
            // 这里是连接数量没有到达上线
            MySQL *p = new MySQL();
            p->connect(_ip, 3306, _user, _password, _dbname);
            p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
            _connectionQue.push(p);
            _connectionCnt++;
        }
        // 通知消费者线程可以消费，若是到达了最大值，也唤醒，因为可能有线程已经用完连接返回了
        _cv.notify_all();
    }
}
// 给外部提供接口，提供一个可用的空闲连接,消费者线程，消费者只会等待若干秒
shared_ptr<MySQL> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty())
    {
        // 条件变量等待超时时间
        if (cv_status::timeout == _cv.wait_for(lock, chrono::microseconds(_connectionTimeout)))
        {
            // 若果是正常返回，说明真的超时了
            if (_connectionQue.empty())
            {
                LOG_INFO << "获取空闲连接超时了....获取连接失败!";
                return nullptr;
            }
        }
        else
        {
        } // notimeout，再检查一次
    }
    // 这里自定义删除器是因为我们不是要真正删除，而是归还到queue当中
    shared_ptr<MySQL> sp(_connectionQue.front(), [&](MySQL *pcon)
                              {
		unique_lock<mutex> lock(_queueMutex);
		pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		_connectionQue.push(pcon); });
    _connectionQue.pop();
    _cv.notify_all();
    return sp;
}

// // 从配置文件加载配置项
// bool ConnectionPool::loadConfigFile()
// {
//     FILE *pf = fopen("mysql.ini", "r");
//     if (pf == nullptr)
//     {
//         LOG("mysql.ini file is not exit");
//         return false;
//     }
//     // 如果文件存在
//     while (!feof(pf))
//     {
//         char line[1024] = {0};
//         fgets(line, 1024, pf);
//         string str = line;
//         // 从0开始找=号
//         int idx = str.find('=', 0);
//         if (idx == -1) // 无效配置项
//         {
//             continue;
//         }
//         // 会有回车 \n
//         int endidx = str.find('\n', idx);
//         string key = str.substr(0, idx);
//         string value = str.substr(idx + 1, endidx - idx - 1);
//         /*cout << key << " " << value << endl;*/

//         if (key == "ip")
//         {
//             _ip = value;
//         }
//         else if (key == "port")
//         {
//             _port = atoi(value.c_str());
//         }
//         else if (key == "username")
//         {
//             _username = value;
//         }
//         else if (key == "password")
//         {
//             _password = value;
//         }
//         else if (key == "dbname")
//         {
//             _dbname = value;
//         }
//         else if (key == "initSize")
//         {
//             _initSize = atoi(value.c_str());
//         }
//         else if (key == "maxSize")
//         {
//             _maxSize = atoi(value.c_str());
//         }
//         else if (key == "maxIdleTime")
//         {
//             _maxIdletime = atoi(value.c_str());
//         }
//         else if (key == "ConnectionTimeOut")
//         {
//             _connectionTimeout = atoi(value.c_str());
//         }
//     }
//     return true;
// }
