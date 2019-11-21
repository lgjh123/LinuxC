#include <iostream>
//#include "logging/Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
    :loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie()),
    //这一步初始化，返回了一个套接字。构造了一个Socket对象
    acceptChannel_(loop,acceptSocket_.fd()),
    //用loop 和上一步构造的socket的的fd构造了channel
    listenning_(false),
    ideal_(::open("/dev/null",O_WRONLY))
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
                 printf("closeeeeeeeeeeeee<<<<<<<<<<<<<<<<?\n");
                 sockets::close(connfd);
            }
        }else if(connfd < 0 ){
           if(errno == EMFILE){
               printf("max fd>>>>>>>>>>>>>>>>\n");
                ::close(ideal_);
                ideal_ = ::accept(acceptSocket_.fd(), NULL, NULL);
                ::close(ideal_);
                ideal_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            }        
        }
    
}
