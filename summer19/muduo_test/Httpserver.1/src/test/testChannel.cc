#include "../net/Channel.h"
#include "../net/EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>

EventLoop* g_loop;

void timeout()
{
    printf("timeout!\n");
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    //这个channel 负责timerfd
    channel.setReadCallback(timeout);
    channel.enableReading();
    //开启读事件(channel --> loop --> poller update():放入pollfdlist,建立channel fd键值对)

    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd,0,&howlong,NULL);

    loop.loop();

    close(timerfd);
}
