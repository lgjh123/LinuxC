#include "TcpConnection.h"

//#incldue "logging/Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"

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
    //??set....Callback~
    //
    //
    //
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
    if(n > 0)
    {
        messageCallback_(shared_from_this(), buf ,n);
    //FIXME :close connection if n==0
    }else if( n == 0)
    {
        handleClose();
    }else{
        handleError();
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    std::cout << "TcpConnection::handleClose state = "
        << state_ << std::endl;
    assert(state_== kConnected);
    //不关闭fd，channel->poll 忽略这个fd
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(channel_->fd());
    std::cout << "TcpConnetion::handleError [ "
        << name_ << "] - SO_ERROR = " << err << " "
        << strerror(err) <<std::endl; //FIXME
}

void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();
  connectionCallback_(shared_from_this());

  loop_->removeChannel(get_pointer(channel_));
}
