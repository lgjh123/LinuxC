#pragma once
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

//channel 时间分发器
//每个channel对象自始至终只属于一个EventLoop
//因此每个channel对象只属于某一个io线程
//每个channel对象自始至终只负责一个fd的时间分发
//但他并不拥有这个fd，也不会在析构的时候关掉这个fd
//channel会把不同的io事件发给不同的回调 
class EventLoop;

class Channel : boost::noncopyable 
{
public:
    typedef boost::function<void()> EventCallback;
    //用function做回调函数
    
    Channel(EventLoop* loop,int fd);
    ~Channel();
    void handleEvent();
    //回调分发函数
    //设置各种回调事件
    void setReadCallback(const EventCallback& cb)
    {   readCallback_ = cb;  }
    void setWriteCallback(const EventCallback& cb)
    {   writeCallback_ = cb;  }
    void setErrorCallback(const EventCallback& cb)
    {   errorCallback_ = cb;  }
    void setCloseCallback(const EventCallback&cb)
    {   closeCallback_ = cb; }
    //Tcpconnection中也有set...Callback
    //channel中的回调是根据poll返回的event触发的

    int fd() const { return fd_; } //获取fd
    int events() const { return events_; } //获取监听事件
    void set_revents(int revt) {revents_ = revt;} //设置返回事件？？不是应该设置需要监听的事件吗？
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    //判断有没有事件
    
    void enableReading() { events_ |= kReadEvent;update(); }
    //update() --> Eventloop --> Poller
    //监听读事件
    void disableAll() { events_ = kNoneEvent; update(); }
    
    
    //poller用来的
    int index() { return index_; } 
    //返回这个channel的索引 
    //index= static_cast<int>(pollfds_.size())-1
    // PollFdList pollfds_
    // PollFdList 是一个存放pollfd结构体的vector
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() { return loop_; }
    //当前channel所在的EventLoop


private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop*  loop_;
    const int   fd_;
    int         events_;    //监听事件channel设置传给Poller
    int         revents_;   //Poller接收,上有set函数可以设置
    int         index_;  //索引,由Poller设置
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;

    bool eventHanding_;
    EventCallback closeCallback_;
};

