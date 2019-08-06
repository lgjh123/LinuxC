#pragma once

#include <boost/noncopyable.hpp>

class InetAddress;

class Socket : boost::noncopyable
{
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd)
    {  }

    ~Socket();
    
    int fd() const { return sockfd_; }
    //如果address被使用出abord
    void bindAddress(const InetAddress& localaddr);

    void listen();

    int accept(InetAddress* peeraddr);

    //Enable/disable SO_REUSEADDR ??
    void setReuseAddr(bool on);

    void shutdownWrite();

private:

    const int sockfd_;
};

