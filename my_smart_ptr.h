#ifndef _MY_SHARED_PTR_H_
#define _MY_SHARED_PTR_H_

#include <iostream>
#include <utility>
#include <stdexcept>
#include <limits>

#include "locker.h"

template<typename T>
class my_shared_ptr
{
public:
    typedef void(*deleter_func)(T*);
    // 构造
    explicit my_shared_ptr(T* _ptr = nullptr, deleter_func _deleter = nullptr) : m_ptr(_ptr), deleter(_deleter)
    {
        if (m_ptr)
        {
            m_count = new size_t(1);
        }
        else
        {
            m_count = new size_t(0);
        }

        m_lock = new locker();
    }

    // 拷贝构造
    my_shared_ptr(const my_shared_ptr& _ptr) : m_ptr(_ptr.m_ptr), deleter(_ptr.deleter), m_count(_ptr.m_count), m_lock(_ptr.m_lock)
    {
        inc_count();
    }

    // 拷贝赋值
    my_shared_ptr& operator=(const my_shared_ptr& _ptr)
    {
        if (this != &_ptr)
        {
            dec_count();
            m_ptr = _ptr.m_ptr;
            deleter = _ptr.deleter;
            m_count = _ptr.m_count;
            m_lock = _ptr.m_lock;
            inc_count();
        }
        return *this;
    }

    // 移动构造
    my_shared_ptr(my_shared_ptr&& _ptr) noexcept
        : m_ptr(_ptr.m_ptr), deleter(_ptr.deleter), m_count(_ptr.m_count), m_lock(_ptr.m_lock)
    {
        _ptr.m_ptr = _ptr.deleter = _ptr.m_count = _ptr.m_lock = nullptr;
    }

    // 移动赋值
    my_shared_ptr& operator=(my_shared_ptr&& _ptr) noexcept
    {
        if (this != &_ptr)
        {
            dec_count();
            m_ptr = _ptr.m_ptr;
            deleter = _ptr.deleter;
            m_count = _ptr.m_count;
            m_lock = _ptr.m_lock;
            _ptr.m_ptr = _ptr.m_count = _ptr.deleter = _ptr.m_lock = nullptr;
        }
        return *this;
    }

    // 析构
    ~my_shared_ptr()
    {
        dec_count();
    }

public:
    T* operator->() const
    {
        if (!m_ptr)
        {
            throw std::runtime_error("null ptr");
        }

        return m_ptr;
    }

    T& operator*() const
    {
        if (!m_ptr)
        {
            throw std::runtime_error("null ptr");
        }

        return *m_ptr;
    }

public:

    T* get() const
    {
        return m_ptr;
    }

    size_t use_count() const
    {
        size_t count = 0;

        m_lock->lock();
        if (m_count)
        {
            count = *m_count;
        }
        m_lock->unlock();

        return count;
    }

private:
    void inc_count()
    {
        m_lock->lock();
        if (m_count)
        {
            ++(*m_count);
        }
        m_lock->unlock();
    }

    void dec_count()
    {
        m_lock->lock();
        if (m_count)
        {
            --(*m_count);
            if (*m_count == 0)
            {
                if (m_ptr)
                {
                    if (deleter)
                    {
                        deleter(m_ptr);
                    }
                    else
                    {
                        delete m_ptr;
                    }
                }

                delete m_count;
                deleter = nullptr;
                m_lock->unlock();
                delete m_lock;
                
                return;
            }
        }
        m_lock->unlock();
    }

private:
    T* m_ptr;
    size_t* m_count;
    deleter_func deleter;
    locker* m_lock;
};

#endif