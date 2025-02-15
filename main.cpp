#include "my_smart_ptr.h"
#include <iostream>

class Test
{
public:
    Test() { std::cout << "Test构造函数" << std::endl; }
    ~Test() { std::cout << "Test析构函数" << std::endl; }

    void print() { std::cout << "Test测试" << std::endl; }
};

// 测试动态数组
void testDynamicArray() {
    std::cout << "\n--- 测试动态数组 自定义删除器 ---\n";
    int* arr = new int[5]{1, 2, 3, 4, 5};
    my_shared_ptr<int> ptr(arr, [](int* p) {
        std::cout << "删除数组中...\n";
        delete[] p;
    });

    for (int i = 0; i < 5; ++i) {
        std::cout << ptr.get()[i] << " ";
    }
    std::cout << "\n";
}

void* thr_func(void *arg)
{
    Test* p = (Test*)arg;
    p->print();
    return nullptr;
}

void testThreadSafety()
{
    std::cout << "\n--- 测试线程安全 ---\n";
    my_shared_ptr<Test> t(new Test());

    pthread_t tids[100];
    for (int i = 0; i < 10; i++)
    {
        pthread_create(&tids[i], nullptr, thr_func, &t);
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(tids[i], nullptr);
    }
}

int main()
{
    {
        my_shared_ptr<Test> t(new Test);
        t->print();
        
        Test* p = t.get();
        p->print();
        p = nullptr;

        std::cout << t.use_count() << std::endl;
        {
            my_shared_ptr<Test> t2 = t;
            std::cout << t.use_count() << std::endl;
            std::cout << t2.use_count() << std::endl;
        }
        std::cout << t.use_count() << std::endl;
    }

    testDynamicArray();

    testThreadSafety();

    return 0;
}
