#include "../include/ConnectionPool.h"
#include "../include/Connection.h"
#include <iostream>
#include <string.h>
#include <exception>
#include <thread>
#include "public.h"

ConnectionPool * ConnectionPool::pConnectionPool = nullptr;

ConnectionPool::ConnectionPool()
{
    if (!loadConfig())
    {
        throw std::exception();
    }

    for (int i = 0; i < m_ConnectionPoolConfig._initSize; i++)
    {
        Connection *p = new Connection();
        if (p->connect(m_ConnectionPoolConfig._ip, m_ConnectionPoolConfig._port, m_ConnectionPoolConfig._username,
                m_ConnectionPoolConfig._pwd, m_ConnectionPoolConfig.dbname))
        {
            p->update_alive_time();
            m_ConnQueue.push(p);
            m_ConnQueue_size++;
        }
    }

    std::thread worker_produce(std::bind(produceConnection, this));
    worker_produce.detach();

    std::thread worker_scan(std::bind(scanConnection, this));
    worker_scan.detach();
}

ConnectionPool::~ConnectionPool()
{
    if (pConnectionPool != nullptr)
    {
        delete pConnectionPool;
    }
}

ConnectionPool* ConnectionPool::getConnectionPoolInstance()
{
    if (pConnectionPool == nullptr)
    {
        pConnectionPool = new ConnectionPool;
    }

    return pConnectionPool;
}

bool ConnectionPool::loadConfig()
{
    FILE *pf = fopen("../config/ConnectionPoolConfig.ini", "r");
    if (pf == nullptr)
    {
        LOG("loadConfig open error");
        return false;
    }

    while (!feof(pf))
    {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        
        char *value = strpbrk(line, "=");
        if (!value)
        {
            continue;
        }

        *value++ = '\0';

        int idx = 0;
        while (value[idx] != '\0')
        {
            if (value[idx] == '\n')
                value[idx] = '\0';
            idx++;
        }

        if (strcasecmp(line, "ip") == 0)
            m_ConnectionPoolConfig._ip = value;
        else if (strcasecmp(line, "port") == 0)
            m_ConnectionPoolConfig._port = static_cast<unsigned short>(atoi(value));
        else if (strcasecmp(line, "password") == 0)
            m_ConnectionPoolConfig._pwd = value;
        else if (strcasecmp(line, "initSize") == 0)
            m_ConnectionPoolConfig._initSize = atoi(value);
        else if (strcasecmp(line, "maxSize") == 0)
            m_ConnectionPoolConfig._maxSize = atoi(value);
        else if (strcasecmp(line, "username") == 0)
            m_ConnectionPoolConfig._username = value;
        else if (strcasecmp(line, "dbname") == 0)
            m_ConnectionPoolConfig.dbname = value;
        else if (strcasecmp(line, "maxIdleTime") == 0)
            m_ConnectionPoolConfig._maxIdleTime = atoi(value);
        else if (strcasecmp(line, "maxConnTimeOut") == 0)
            m_ConnectionPoolConfig._connTimeout = atoi(value);
    }

    fclose(pf);
    return true;
}

void ConnectionPool::produceConnection(ConnectionPool* const pPool)
{
    // 生产者
    while (true)
    {
        std::unique_lock<std::mutex> lock(pPool->queue_lock);
        while (!pPool->m_ConnQueue.empty())
        {
            pPool->cv.wait(lock);
        }

        if (pPool->m_ConnQueue_size < pPool->m_ConnectionPoolConfig._maxSize)
        {
            Connection *p = new Connection();
            if (p->connect(pPool->m_ConnectionPoolConfig._ip, pPool->m_ConnectionPoolConfig._port, pPool->m_ConnectionPoolConfig._username,
                pPool->m_ConnectionPoolConfig._pwd, pPool->m_ConnectionPoolConfig.dbname))
            {
                p->update_alive_time();
                pPool->m_ConnQueue.push(p);
                pPool->m_ConnQueue_size++;
            }
        }

        // 通知消费者消费连接
        pPool->cv.notify_all();
    }
}

void ConnectionPool::scanConnection(ConnectionPool* const pPool)
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(pPool->m_ConnectionPoolConfig._maxIdleTime));

        std::unique_lock<std::mutex> lock(pPool->queue_lock);
        while (pPool->m_ConnQueue_size > pPool->m_ConnectionPoolConfig._initSize)
        {
            Connection *p = pPool->m_ConnQueue.front();
            if (p->get_alive_time() >= (pPool->m_ConnectionPoolConfig._maxIdleTime * 1000))
            {
                pPool->m_ConnQueue.pop();
                delete p;
                pPool->m_ConnQueue_size--;
            }
            else
            {
                break;
            }
        }
    }
}

// 接口，获得一个连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(queue_lock);
    if (m_ConnQueue.empty())
    {
        if (std::cv_status::timeout ==
                cv.wait_for(lock, std::chrono::milliseconds(m_ConnectionPoolConfig._connTimeout)))
        {
            LOG("fail to get free connect")
            return nullptr;
        }
    }

    std::shared_ptr<Connection> p(m_ConnQueue.front(), 
            [&](Connection *pConn)
            {
                std::unique_lock<std::mutex> lock(queue_lock);
                pConn->update_alive_time();
                m_ConnQueue.push(pConn);
            });

    m_ConnQueue.pop();
    if (m_ConnQueue.empty())
    {
        cv.notify_all();
    }
    return p;
}