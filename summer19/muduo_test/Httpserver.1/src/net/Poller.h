#pragma once
#include <map>
#include <vector>

//#include "datetime/Timestamp.h"
#include "EventLoop.h"

struct epoll_event;

class Channel;

class EPoller : boost::noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    EPoller(EventLoop* loop);
    ~EPoller();

    //轮询IO事件，要在loop线程中调用（要和EventLoop在一个线程）
    //Timestamp poll(int timeoutMs,ChannelList* activeChannels);
    void poll(int timeoutMs,ChannelList* activeChannels);
//timestamp还未实现??
    void updateChannel(Channel* channel);
    //更新(添加)感兴趣事件,在自己的EventLoop线程中调用
    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }
    //判断是否在自己的loop线程中调用
    void removeChannel(Channel* channel);

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannel(int numEvents,
                           ChannelList* activeChannels);
    //将轮询得到的到达事件存入activechannel中
    void update(int operation, Channel* channel);
    typedef std::vector<struct epoll_event> EventList;
    //保存struct pollfd的vector
    typedef std::map<int,Channel*> ChannelMap;
    //保存fd和channel的键值对
    
    int epollfd_;
    EventLoop* ownerLoop_;
    //指向传入eventloop的指针，可以判断是否在它执行的线程中
    EventList events_;
    //保存struct pollfd的可变长数组
    ChannelMap channels_;


};

