#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <mysql/mysql.h>
#include <time.h>

class Connection
{
public:
    Connection();
    ~Connection();

public:
    bool connect(std::string ip, unsigned short port, std::string user, 
            std::string pwd, std::string dbname);

    bool update(std::string sql);
    MYSQL_RES* query(std::string sql);

    void update_alive_time() { _alive_time = clock(); }
    clock_t get_alive_time() const { return clock() - _alive_time; }
private:
    MYSQL *_conn;
    clock_t _alive_time;
};

#endif