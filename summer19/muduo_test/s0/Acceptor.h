#pragma once
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "Channel.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor : boost::noncopyable
{
public:
    typedef boost::function<void (int sockfd,
                                  const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop,const InetAddress& listenAddr);

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop* loop_;
    //数据成员包括Socket、Channel
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;

};

