#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <pthread.h>
#include <stdexcept>

class locker
{
public:
    locker()
    {
        if (pthread_mutex_init(&mut, nullptr) != 0)
            throw std::runtime_error("init mutex error");
    }

    locker(const locker&) = delete;
    locker& operator=(const locker&) = delete;

    locker(locker&& other) noexcept
    {
        mut = other.mut;
        other.mut = pthread_mutex_t();
    }

    locker& operator=(locker&& other) noexcept
    {
        if (this != &other)
        {
            pthread_mutex_destroy(&mut);
            mut = other.mut;
            other.mut = pthread_mutex_t();
        }

        return *this;
    }

    ~locker()
    {
        pthread_mutex_destroy(&mut);
    }

public:
    void lock()
    {
        if (pthread_mutex_lock(&mut) != 0)
            throw std::runtime_error("lock mutex error");
    }

    void unlock()
    {
        if (pthread_mutex_unlock(&mut) != 0)
            throw std::runtime_error("unlock mutex error");
    }

private:
    pthread_mutex_t mut;
};

#endif