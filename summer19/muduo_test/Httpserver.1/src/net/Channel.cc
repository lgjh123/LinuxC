#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

//loop 传参用的局部变量 loop_真正channel的变量
Channel::Channel(EventLoop* loop,int fdArg)
    : loop_(loop),
    fd_(fdArg),
    events_(0),
    revents_(0),
    index_(-1),
    eventHanding_(false)
{   }
Channel::~Channel()
{
    assert(!eventHanding_);
}
void Channel::update()
{
    loop_->updateChannel(this);
}


void Channel::handleEvent()
{
    eventHanding_ = true;
    if(revents_ & POLLNVAL){
        std::cout << "something warn POLLNVAL" << std::endl;
        exit(1);
    }
    if((revents_& POLLHUP) && !(revents_ & POLLIN)){
        std::cout << "Channel::handle_event() POLLHUP " 
            <<std::endl;
        if(closeCallback_) closeCallback_();
    }

    if(revents_ &(POLLERR | POLLNVAL)){
        if(errorCallback_) errorCallback_();
    }
    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readCallback_) readCallback_();
        std::cout << "POLLIN! " << std::endl;
    }
    if(revents_ & POLLOUT ){
        if(writeCallback_) writeCallback_();
    }

    eventHanding_ = false;
}

