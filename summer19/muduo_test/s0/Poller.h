#pragma once
#include <map>
#include <vector>

//#include "datetime/Timestamp.h"
#include "EventLoop.h"

struct pollfd;

class Channel;

class Poller : boost::noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller();

    //轮询IO事件，要在loop线程中调用（要和EventLoop在一个线程）
    //Timestamp poll(int timeoutMs,ChannelList* activeChannels);
    void poll(int timeoutMs,ChannelList* activeChannels);
//timestamp还未实现??
    void updateChannel(Channel* channel);
    //更新(添加)感兴趣事件,在自己的EventLoop线程中调用
    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }
    //判断是否在自己的loop线程中调用

private:
    void fillActiveChannel(int numEvents,
                           ChannelList* activeChannels);
    //将轮询得到的到达事件存入activechannel中
    typedef std::vector<struct pollfd> PollFdList;
    //保存struct pollfd的vector
    typedef std::map<int,Channel*> ChannelMap;
    //保存fd和channel的键值对
    
    EventLoop* ownerLoop_;
    //指向传入eventloop的指针，可以判断是否在它执行的线程中
    PollFdList pollfds_;
    //保存struct pollfd的可变长数组
    ChannelMap channels_;


};

