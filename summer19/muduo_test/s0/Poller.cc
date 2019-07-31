#include <iostream>
#include "Poller.h"
#include "Channel.h"

#include <assert.h>
#include <poll.h>

//顾名思义拥有它的loop,调用它的EventLoop
//后面需要用这个指针判断是否在一个线程中执行
Poller::Poller(EventLoop* loop)
        : ownerLoop_(loop)
{
}

//空的析构韩式？
Poller::~Poller()
{
}

//Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
void Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(),
                           pollfds_.size(),timeoutMs);
    //获得到达事件的个数
    //Timestamp now(Timesstamp::now());
    if(numEvents > 0){
        std::cout << "numEvent happended" << std::endl;
        fillActiveChannel(numEvents,activeChannels);
    }else if(numEvents == 0){
        std::cout << "nothing happended" <<std::endl;
    }else{
        std::cout << "sys_errno Poller:poll" <<std::endl;
    }
    //return now;
}

void Poller::fillActiveChannel(int numEvents,
                               ChannelList* activeChannels)
{
    for(PollFdList::const_iterator pfd = pollfds_.begin();
    pfd != pollfds_.end() && numEvents > 0;++pfd)
    //numEvents = 0时说明到达事件已经添加完
    {
       if(pfd->revents > 0) //返回事件不为0
       {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            //用结构体中的fd找到fd所在的channel
            //struct poller中返回的只有fd event revent
            //一个fd一个channel需要查找，fd相应的事件分发器channel
            assert(ch != channels_.end());
            Channel*channel = ch->second;
            //将该fd的channel取出
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            //将得到的到达（返回）事件类型传给fd对应的channel
            //pfd->revents = 0;
            activeChannels->push_back(channel);
            //将刚得到设置好的channel加入channellist容器中
            //用于之后在loop中统一回调
       }

    }
}
void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    std::cout << "fd = " << channel->fd() <<" events = " << channel->events();
    if(channel->index() < 0)
    {
        //如果channe的索引
    }
}
