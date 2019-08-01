#pragma once
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <sys/types.h>
#include <sys/syscall.h>

class Channel;
class Poller;

class EventLoop : boost::noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void updateChannel(Channel* channel);
    
     void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const {return threadId_ == syscall(SYS_gettid) ;} 

private:
    void abortNotInLoopThread();
    boost::scoped_ptr<Poller> poller_;
    typedef std::vector<Channel*> ChannelList;
    
    bool looping_;
    bool quit_;
    const pid_t threadId_;
    ChannelList activeChannels_;

};

