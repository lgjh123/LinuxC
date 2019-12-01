#include <iostream>
#include "Poller.h"
#include "Channel.h"

#include <assert.h>
#include <poll.h>
#include <boost/implicit_cast.hpp>
#include <sys/epoll.h>
#include <boost/static_assert.hpp>
#include <errno.h>

BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

namespace{    //namespace用法
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

//顾名思义拥有它的loop,调用它的EventLoop
//后面需要用这个指针判断是否在一个线程中执行
EPoller::EPoller(EventLoop* loop)
        : ownerLoop_(loop),
          epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
          events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        printf("EPoller::EPoller\n");
    }
}

//空的析构韩式？
EPoller::~EPoller()
{
}

//Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
void EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_,
                           events_.data(),
                           static_cast<int>(events_.size()),
                           timeoutMs);
    //获得到达事件的个数
    //Timestamp now(Timesstamp::now());
    if(numEvents > 0){
        std::cout << "events happended" << std::endl;
        fillActiveChannel(numEvents,activeChannels);
        if(boost::implicit_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size()*2);
        }
    }else if(numEvents == 0){
        std::cout << "nothing happended" <<std::endl;
    }else{
        std::cout << "sys_errno Poller:poll" <<std::endl;
    }
    //return now;
}

void EPoller::fillActiveChannel(int numEvents,
                               ChannelList* activeChannels)
{
    //poll每次需要遍历O(n)    epoll每次查找只需O(logn)
    assert(boost::implicit_cast<size_t>(numEvents)<=events_.size());

    for(int i = 0;i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
#endif
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }

}
void EPoller::updateChannel(Channel* channel)
{
        //channe索引的必要性?
        assertInLoopThread();
        std::cout << ">>>>UPDATECHANNEL--------fd = " << channel->fd() << " events = " <<channel->events() << std::endl;
        const int index = channel->index();

        if(index == kNew || index == kDeleted)
        {
            //a new one ,add with EPOLL_CTL_ADD
            int fd = channel->fd();
            if(index == kNew)
            {
                assert(channels_.find(fd) == channels_.end());
                channels_[fd] = channel;
            }
            else // index == kDeleted
            {
                assert(channels_.find(fd) != channels_.end());
                assert(channels_[fd] == channel );
            }
            channel->set_index(kAdded);
            update(EPOLL_CTL_ADD,channel);
        }
        else
        {
            //更新一个存在的fd with EPOLL_CTL_MOD
            int fd = channel->fd();
            (void)fd;
            //assert ???
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
            assert(index == kAdded);
            if(channel->isNoneEvent())
            {
                update(EPOLL_CTL_DEL,channel);
                channel->set_index(kDeleted);
            }
            else
            {
                update(EPOLL_CTL_MOD,channel);
            }
        }

}
void EPoller::removeChannel(Channel* channel)
{
    assertInLoopThread();
    std::cout << "ooooooooooooooooooooremove Channel fd = \n" << channel->fd();
    int fd = channel->fd();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);

    size_t n = channels_.erase(fd);
    (void)n;         //????????
    assert(n == 1);

    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(kNew);

}
void EPoller::update(int operation, Channel* channel)
{
  struct epoll_event event;
  bzero(&event, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
  {
    if (operation == EPOLL_CTL_DEL)
    {
      std::cout << "epoll_ctl op=" << operation << " fd=" << fd;
    }
    else
    {
      std::cout << "epoll_ctl op=" << operation << " fd=" << fd;
    }
  }
}