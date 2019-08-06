#include <iostream>
//#include "logging/Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>

Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
    :loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie()),
    //这一步初始化，返回了一个套接字。构造了一个Socket对象
    acceptChannel_(loop,acceptSocket_.fd()),
    //用loop 和上一步构造的socket的的fd构造了channel
    listenning_(false)
{
    //在Socket中调用相关函数
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(
                boost::bind(&Acceptor::handleRead,this));
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    //将套接字设置为监听套接字
    acceptChannel_.enableReading();
    //监听读事件
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    //FIXME loop until no more
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0)
    {
        if(newConnectionCallback_){
            //如果定义了回调
            newConnectionCallback_(connfd,peerAddr);
        }else{
            sockets::close(connfd);
        }
    }
}
