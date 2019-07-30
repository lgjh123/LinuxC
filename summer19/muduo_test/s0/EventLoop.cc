#include <iostream>
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>

EventLoop::EventLoop()
    : looping_(false),
    quit_(false),
    threadId(syscall(SYS_gettid))
void abortNotInLoopThread()
{
    std::cout << "EventLoop::abrotNotInloopThread" << std::endl;
    exit(0);
}
