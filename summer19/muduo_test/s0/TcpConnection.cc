#include "TcpConnection.h"

//#incldue "logging/Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#include <boost/bind.hpp>

#include <errno.h>
#include <stdio.h>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    :loop_(loop), //check not null
     name_(nameArg),
     state_(kConnecting),
     socket_(new Socket(sockfd)),
     channel_(new Channel(loop, sockfd)),
     localAddr_(localAddr),
     peerAddr_(peerAddr)
{
    std::cout << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd
            <<std::endl;
    channel_->setReadCallback(
         boost::bind(&TcpConnection::handleRead, this));
    //绑定回调函数
}

TcpConnection::~TcpConnection()
{
    std::cout << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd()
            << std::endl;
}

//TcpServer accepts a new connection(socket)
void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf ,sizeof buf);
    messageCallback_(shared_from_this(), buf ,n);
    //FIXME :close connection if n==0
}
