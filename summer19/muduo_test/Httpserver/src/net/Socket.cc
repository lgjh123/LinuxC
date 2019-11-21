#include <iostream>

#include "Socket.h"

#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>

Socket::~Socket()
{
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bindOrDie(sockfd_,addr.getSockAddrInet());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof addr);
   // int connfd = sockets::accept(sockfd_,&addr);
    socklen_t len = sizeof(addr);
    int connfd =  ::accept(sockfd_,(struct sockaddr*)&addr,&len);
    if(connfd >= 0)
    {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,
                 &optval,sizeof optval);
    //FIXME CHECK
}

void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}

