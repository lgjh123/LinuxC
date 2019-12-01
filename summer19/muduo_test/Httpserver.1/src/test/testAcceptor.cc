#include "../net/Acceptor.h"     
#include "../net/EventLoop.h"
#include "../net/InetAddress.h"
#include "../net/SocketsOps.h"
#include <stdio.h>

void newConnection(int sockfd, const InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
         sockets::close(sockfd);
}
void newConnection1(int sockfd,const InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "Hellohello\n", 13);
         sockets::close(sockfd);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(9981);
  InetAddress listenAddr1(9980);
  EventLoop loop;

  Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  Acceptor acceptor1(&loop, listenAddr1);
  acceptor1.setNewConnectionCallback(newConnection1);
  acceptor1.listen();

  loop.loop();
}
