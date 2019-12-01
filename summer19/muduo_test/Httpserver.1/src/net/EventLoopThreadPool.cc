// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <boost/bind.hpp>



EventLoopThreadPool::EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  : baseLoop_(baseLoop),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start()
{
  assert(!started_);
  //printf("evpool---------------**********--------------\n");
  baseLoop_->assertInLoopThread();
  //printf("evpool---------------------------888888\n");

  started_ = true;

  for (int i = 0; i < numThreads_; ++i)
  {
    EventLoopThread* t = new EventLoopThread;
    threads_.push_back(t);
    loops_.push_back(t->startLoop());
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  //printf("nextlooppppppppppp0-0-0-0-0-0-0-\n");
  baseLoop_->assertInLoopThread();
  //printf("nextlooppppppppppp1-1-11-1-1-1-1-1-\n");
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (static_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}

