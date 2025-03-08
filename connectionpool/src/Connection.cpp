#include "../include/Connection.h"
#include "../include/public.h"

#include <iostream>

Connection::Connection()
{
    _conn = mysql_init(nullptr);
}

Connection::~Connection()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn);
    }
}

bool Connection::connect(std::string ip, unsigned short port, std::string user, 
        std::string pwd, std::string dbname)
{
    return nullptr != 
            mysql_real_connect(_conn, ip.c_str(), user.c_str(), pwd.c_str(),
                    dbname.c_str(), port, nullptr, 0);                              
}

bool Connection::update(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG(std::string("更新失败:") + sql);
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG(std::string("查询失败:") + sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}