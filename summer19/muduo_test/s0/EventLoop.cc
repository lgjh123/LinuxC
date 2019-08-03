#include <iostream>
#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>

#include <sys/eventfd.h>

__thread EventLoop* t_looInThisThread = 0;
const int kPollTimeMs = 10000;

static int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC );
    if(evtfd < 0)
    {
        std::cout << "Failed in eventfd " << std::endl;
        exit(1);
    }
    return evtfd;
}
EventLoop::EventLoop()
    : looping_(false),
    quit_(false),
    threadId_(syscall(SYS_gettid)),
    poller_(new Poller(this)) ,// pool是多路复用的封装

    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this,wakeupFd_))

   // cout 不是线程安全的
{
    std::cout << "Eventloop created " << this <<std::endl;
    if(t_looInThisThread)
    {
        std::cout << "EventLoop exists"<< std::endl; 
        assert(!t_looInThisThread);
    }
    else
    {
        t_looInThisThread = this;
    }
    
}
EventLoop::~EventLoop()
{
    assert(!looping_);
    t_looInThisThread = NULL;
}
void EventLoop::loop()
{
    assert(!looping_); //loop不再进行中
    assertInLoopThread(); //check loop进行的线程和EventLoop线程是否相同
    looping_ = true;
    quit_ = false;  //控制循环

    while(!quit_)
    {
        activeChannels_.clear(); //清空容器(初始化)
        poller_->poll(kPollTimeMs,&activeChannels_);
        //用poll获取活跃的IO事件？
        for(ChannelList::iterator it = activeChannels_.begin();
            it != activeChannels_.end(); ++it)
        {
           // std::cout << "test" << std::endl;quit();
          (*it)->handleEvent();
        }//遍历活跃io时间，执行回调
    }
    std::cout << "EventLoop " << this << " stop looping"<<std::endl;
    looping_ = false;
}
void EventLoop::quit()
{
    quit_ = true;
    //quit()不会立刻停止loop，而是等执行完一次loop
    //再判断while条件时退出
}
void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    //判断channel是不是此Eventloop的channel
    //ps:channel使用时要传入loop的指针
    assertInLoopThread();
    //判断当前线程是否是EventLoop所在线程
    poller_->updateChannel(channel);

}
void EventLoop::abortNotInLoopThread()
{
    std::cout << "EventLoop::abrotNotInloopThread" << std::endl;
    assert(false);
}





void EventLoop::runInLoop(const Functor& cb)
{
  if (isInLoopThread())
  {
    cb();
  }
  else
  {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(const Functor& cb)
{
  {
  mutex.lock();
  //FIXME
  pendingFunctors_.push_back(cb);
  }

  if (!isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();
  }
}



void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof one)
  {
    std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8" 
    << std::endl;
  }
}

void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof one)
  {
    std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8"
    << std::endl;
  }
}

void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
  mutex.lock();
  //FIXME
  functors.swap(pendingFunctors_);
  }

  for (size_t i = 0; i < functors.size(); ++i)
  {
    functors[i]();
  }
  callingPendingFunctors_ = false;
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->removeChannel(channel);
}