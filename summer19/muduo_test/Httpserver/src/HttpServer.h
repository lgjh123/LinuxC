#include "TcpServer.h"
#include "boost/noncopyable.hpp"

class HttpRequest;
class HttpResponse;

class HttpServer : boost::noncopyable
{
 public:
 // typedef std::function<void (const HttpRequest&,
  //                            HttpResponse*)> HttpCallback;

  HttpServer(EventLoop* loop,
             const InetAddress& listenAddr);


  /// Not thread safe, callback be registered before calling start().
 /* void setHttpCallback(const HttpCallback& cb)
  {
    httpCallback_ = cb;
  }*/
  void setResponse(const HttpRequest& req,HttpResponse* resp);


  void start();

 private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 int receiveTime);
  void onRequest(const TcpConnectionPtr&, const HttpRequest&);

  TcpServer server_;
 // HttpCallback httpCallback_;
};
