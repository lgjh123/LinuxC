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
        //channe索引的必要性?
        assertInLoopThread();
        std::cout << "fd = " << channel->fd() << " events = " <<channel->events() << std::endl;
        //索引小于零，说明之前不在channel map中
        //是一个新的channel
        if(channel->index() < 0 )
        {
            assert(channels_.find(channel->fd()) == channels_.end());
            struct pollfd pfd;
            //为它分配struct pollfd 
            pfd.fd = channel->fd();
            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
            pollfds_.push_back(pfd);
            //向pollfd list加入这个channel的相关信息
            int idx = static_cast<int>(pollfds_.size())-1;
            channel->set_index(idx);
            channels_[pfd.fd] = channel;
            //建立fd与channel的键值对
        }
        else
        {
            //update一个已经在channelmap中的channel
            assert(channels_.find(channel->fd()) != channels_.end());
            //在channel中可以找到
            assert(channels_[channel->fd()] == channel);
            //更新的channel可以在索引中找到
            int idx = channel->index();
            assert(0 <= idx&& idx < static_cast<int>(pollfds_.size()));
            struct pollfd& pfd = pollfds_[idx];
            //pfd是pollfds_[idx]的引用
            assert(pfd.fd == channel->fd() || pfd.fd == -1);
            //根据该channel的index找到相应的pollfd
            //它的fd可能与channel相同，也可能被设定为-1（忽略）
            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
            //更新该pollfd的信息
            if(channel->isNoneEvent())
            {
                //忽略这个pollfd
                pfd.fd = -1;
            }
        }
    }
}
//pollfdlist通过index维护
//每个channel都有自己的index，但是它对应的fd
//可能在pollfd中被设置为-1
//但在channelmap中的关系不变
