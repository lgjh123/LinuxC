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

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }
    //channel也有回调
    //但这里的回调是面向一个连接的从callback中调用
    //可以从外部调用的设置?

    void connectEstablished();  
    //在accept到新的connection(socket)时被调用
    //且只被调用一次
    void connectDestroyed(); //只被调用一次
        
    //当Tcpserver从他的map中removed 这个Tcpconnetion调用


private:
    enum StateE { kConnecting, kConnected ,kDisconnected };

    void setState(StateE s) { state_ = s; }
    void handleRead();
    //handleRead会检查read的返回值，根据返回值分别调用
    //messagecallback、handleClose、handleError
    void handleWrite();
    void handleClose();
    void handleError();


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
    CloseCallback closeCallback_;

};
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

