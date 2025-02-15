#ifndef _MY_SMART_PTR_H_
#define _MY_SMART_PTR_H_

#include <iostream>
#include <utility>

template<class T>
class my_smart_ptr
{
public:
    // 构造
    explicit my_smart_ptr(T* _ptr = nullptr)
    {
        m_ptr = _ptr;
        if (m_ptr)
            m_count = new size_t(1);
        else
            m_count = new size_t(0);
    }

    // 拷贝构造
    my_smart_ptr(const my_smart_ptr& _ptr) : m_ptr(_ptr.m_ptr), m_count(_ptr.m_count)
    {
        inc_count();
    }

    // 拷贝赋值
    my_smart_ptr& operator=(const my_smart_ptr& _ptr)
    {
        if (this != _ptr)
        {
            dec_count();
            m_ptr = _ptr.m_ptr;
            m_count = _ptr.m_count;
            inc_count();
        }
        return *this;
    }

    // 移动构造
    my_smart_ptr(my_smart_ptr&& _ptr) noexcept
        : m_ptr(_ptr.m_ptr), m_count(_ptr.m_count)
    {
        _ptr.m_ptr = _ptr.m_count = nullptr;
    }

    // 移动赋值
    my_smart_ptr& operator=(my_smart_ptr&& _ptr) noexcept
    {
        if (this != _ptr)
        {
            dec_count();
            m_ptr = _ptr.m_ptr;
            m_count = _ptr.m_count;
            _ptr.m_ptr = _ptr.m_count = nullptr;
        }
        return *this;
    }

    // 析构
    ~my_smart_ptr()
    {
        dec_count();
    }

public:
    T* operator->() const
    {
        if (!m_ptr)
        {
            throw std::run_timeerror("null ptr");
        }

        return m_ptr;
    }

    T& operator*() const
    {
        if (!m_ptr)
        {
            throw std::run_timeerror("null ptr");
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
        int count = m_count ? *m_count : 0;
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
            if (*m_count == 0)
            {
                delete m_ptr;
                delete m_count;
                return;
            }
        }
    }

private:
    T* m_ptr;
    size_t* m_count;
};

#endif