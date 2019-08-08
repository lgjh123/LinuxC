#include "TcpConnection.h"

//#incldue "logging/Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>
#include <boost/implicit_cast.hpp>
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
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if(n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_ , 0);
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
    assert(state_== kConnected || state_ == kDisconnecting);
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

void TcpConnection::send(Buffer* buf)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread()) 
        {
            sendInLoop(buf->peek(),buf->readableBytes());
        }
        else
        {
           // loop_->runInLoop(boost::bind(&TcpConnection::sendInLoop,this,buf->retrieveAsString()));
        }
    }
}

void TcpConnection::sendInLoop(const void* data,size_t len)
{
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {  //不监控可读时间且可读区buffer没有数据
        nwrote = ::write(channel_->fd(),data,len);
        if(nwrote >= 0)
        {
            if(boost::implicit_cast<size_t>(nwrote) < len)
                std::cout << "I am going to write more data "<<std::endl;
            //这次写完还有剩余数据
        }
        else
        {
            nwrote = 0;
            if(errno!= EWOULDBLOCK)
                std::cout << "TcpConnection::sendInLoop" << std::endl;
        }
        
    }
    assert(nwrote >= 0);
    if (boost::implicit_cast<size_t>(nwrote) <len)
    {
        outputBuffer_.append(data+nwrote, len-nwrote);
        //将发完剩余的内容加入buffer中
        if (!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }    
}

void TcpConnection::shutdown()
{
    //FIXME: use compare and swap
    if(state_ == kConnected)
    {
        setState(kDisconnecting);
        //FIXME: share_from_this()?
        loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop,this));
    }
}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  if (!channel_->isWriting())
  {
    // we are not writing
    socket_->shutdownWrite();
  }
}


void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if (channel_->isWriting())
  { //如果监听了读事件
    ssize_t n = ::write(channel_->fd(),
                        outputBuffer_.peek(),
                        outputBuffer_.readableBytes());
    if (n > 0) 
    {
        outputBuffer_.retrieve(n);
        if (outputBuffer_.readableBytes() == 0) 
        { //如果buffer中的数据被读完，取消对可读时间的监控
            channel_->disableWriting();
            if (state_ == kDisconnecting)
            {
            shutdownInLoop();
            }
        }
        else 
        {
            std::cout << "I am going to write more data" << std::endl;
        }
    }
    else
    {
      std::cout << "TcpConnection::handleWrite" <<std::endl;
    }
  } 
  else 
  {
    std::cout << "Connection is down, no more writing"<<std::endl;
  }
}
