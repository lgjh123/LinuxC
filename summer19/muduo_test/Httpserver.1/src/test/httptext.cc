#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <stdio.h>
#include "Channel.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

std::string message1("HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n");
std::string message2("<html><body>hello http</body></html>");

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
    conn->send(message1);
    conn->send(message2);
    conn->shutdown(); 
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               int receiveTime)
{
  printf("onMessage(): received %zd bytes from connection [%s] at \n",
         buf->readableBytes(),
         conn->name().c_str());
         //receiveTime.toFormattedString().c_str());
  std::string m("in message\n");
  conn->send(m);
  buf->retrieveAll();
}

int main(int argc, char* argv[])
{
  printf("main(): pid = %d\n", getpid());


  InetAddress listenAddr(9981);
  EventLoop loop;

  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}
