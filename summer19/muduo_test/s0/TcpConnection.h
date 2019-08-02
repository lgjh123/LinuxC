#pragma once
#include "Callbacks.h"
#include "InetAddress.h"

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

class Channel;
class EventLoop;
class Socket;

class TcpConnection : boost::noncopyable,
                      public boost::enable_shared_from_this<TcpConnection>
{
public:
    //构建一个TcpConnection 用一个已经Connection的sockfd
    //没有可供用户使用的接口
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() { return localAddr_; }
    const InetAddress& peerAddress() { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    
    void connectEstablished();  
    //在accept到新的connection(socket)时被调用
    //且只被调用一次


private:
    enum StateE { kConnecting, kConnected };

    void setState(StateE s) { state_ = s; }
    void handleRead();

    EventLoop* loop_;
    std::string name_;
    StateE state_; //FIXME

    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    //不能把这些class暴露给用户
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;

};
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

