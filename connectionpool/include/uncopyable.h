#ifndef UNCOPY_ABLE_H_
#define UNCOPY_ABLE_H_

class uncopyable
{
public:
    uncopyable(const uncopyable&) = delete;
    uncopyable& operator=(const uncopyable&) = delete;

protected:
    uncopyable() = default;
    ~uncopyable() = default;
};

#endif