#ifndef LOCKER_H_
#define LOCKER_H_

#include "uncopyable.h"

#include <pthread.h>
#include <exception>

class locker : public uncopyable
{
public:
    locker()
    {
        if (pthread_mutex_init(&mut, NULL) != 0)
        {
            throw std::exception();
        }
    }

    ~locker()
    {
        if (pthread_mutex_destroy(&mut) != 0)
        {
            throw std::exception();
        }
    }

public:
    bool lock()
    {
        return pthread_mutex_lock(&mut) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock(&mut) == 0;
    }

private:
    pthread_mutex_t mut;
};

#endif