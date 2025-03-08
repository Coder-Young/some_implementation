#ifndef CONNECTION_POOL_H_
#define CONNECTION_POOL_H_

//#include "locker.h"

#include <string>
#include <queue>
#include <atomic>
#include <memory>
#include <functional>
#include <condition_variable>
#include <mutex>

class Connection;
class ConnectionPool
{
    struct ConnectionPoolConfig
    {
        std::string _ip;
        unsigned int short _port;
        std::string _username;
        std::string _pwd;
        std::string dbname;
    
        int _initSize;
        int _maxSize;
        int _maxIdleTime;
        int _connTimeout;
    };

public:
    static ConnectionPool* getConnectionPoolInstance();

    std::shared_ptr<Connection> getConnection();

private:
    ConnectionPool();
    ~ConnectionPool();

    bool loadConfig();

    // 线程函数
    static void produceConnection(ConnectionPool* const pPool);
    static void scanConnection(ConnectionPool* const pPool);

private:
    static ConnectionPool *pConnectionPool;

private:
    ConnectionPoolConfig m_ConnectionPoolConfig;

    std::queue<Connection*> m_ConnQueue;
    std::atomic_int m_ConnQueue_size;

    std::mutex queue_lock;
    std::condition_variable cv;
};

#endif