#ifndef _MY_SHARED_PTR_H_
#define _MY_SHARED_PTR_H_

#include <iostream>
#include <utility>
#include <stdexcept>
#include <limits>

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
    }

    // 拷贝构造
    my_shared_ptr(const my_shared_ptr& _ptr) : m_ptr(_ptr.m_ptr), deleter(_ptr.deleter), m_count(_ptr.m_count)
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
            inc_count();
        }
        return *this;
    }

    // 移动构造
    my_shared_ptr(my_shared_ptr&& _ptr) noexcept
        : m_ptr(_ptr.m_ptr), deleter(_ptr.deleter), m_count(_ptr.m_count)
    {
        _ptr.m_ptr = _ptr.deleter = _ptr.m_count = nullptr;
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
            _ptr.m_ptr = _ptr.m_count = _ptr.deleter = nullptr;
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
        size_t count = m_count ? *m_count : 0;
        return count;
    }

private:
    void inc_count()
    {
        if (m_count)
        {
            ++(*m_count);
        }
    }

    void dec_count()
    {
        if (m_count)
        {
            --(*m_count);
            if (*m_count <= 0)
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
                
                return;
            }
        }
    }

private:
    T* m_ptr;
    size_t* m_count;
    void(*deleter)(T*);
};

#endif