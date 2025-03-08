#include "../include/Connection.h"
#include "../include/ConnectionPool.h"

#include <thread>
#include <iostream>

void normal_insert(const int insert_size)
{
    for (int i = 1; i <= insert_size; i++)
    {
        char sql[1024] = {0};
        sprintf(sql, "insert into animals ( ID, animal_name ) values ( '%d', '%s' );", i, "pig");
        
        Connection conn;
        conn.connect("127.0.0.1", 3306, "root", "123456", "my_db_1");
        conn.update(sql);
    }
}

void use_pool_insert(const int insert_size)
{
    ConnectionPool *pPool = ConnectionPool::getConnectionPoolInstance();
    for (int i = 1; i <= insert_size; i++)
    {
        char sql[1024] = {0};
        sprintf(sql, "insert into animals ( ID, animal_name ) values ( '%d', '%s' );", i, "pig");
        
        std::shared_ptr<Connection> sp = pPool->getConnection();
        sp->update(sql);
    }
}

void use_multi_thread_pool_insert(const int insert_size)
{
    ConnectionPool *pPool = ConnectionPool::getConnectionPoolInstance();
   
    std::thread t1([&]() {
        for (int i = 1; i <= insert_size / 2; i++)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into animals ( ID, animal_name ) values ( '%d', '%s' );", i, "pig");
            
            std::shared_ptr<Connection> sp = pPool->getConnection();
            sp->update(sql);
        }
    });

    std::thread t2([&]() {
        for (int i = 1; i <= insert_size / 2; i++)
        {
            char sql[1024] = {0};
            sprintf(sql, "insert into animals ( ID, animal_name ) values ( '%d', '%s' );", i, "pig");
            
            std::shared_ptr<Connection> sp = pPool->getConnection();
            sp->update(sql);
        }
    });
}

void delete_rows()
{
    char sql[1024] = {0};
    sprintf(sql, "delete from animals;");
    
    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "my_db_1");
    conn.update(sql);
}

int main()
{
    clock_t begin = clock();


    //normal_insert(1000);
    use_pool_insert(1000);
    //use_multi_thread_pool_insert(1000);

    clock_t end = clock();
    std::cout << (end - begin) << "ms" << std::endl;
    delete_rows();

    return 0;
}