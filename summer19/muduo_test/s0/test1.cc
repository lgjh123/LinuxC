#include <iostream>
#include "EventLoop.h"
#include <thread>
void f1()
{
    EventLoop loop;
    loop.loop();
}
int main()
{
    EventLoop loop;

    loop.loop();
    //sleep(1);
    //loop.quit();
    std::thread t1(f1);
    t1.join();
    return 0;
}

