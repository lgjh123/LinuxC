#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include <stdio.h>

void newConnection(int sockfd, const InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
         sockets::close(sockfd);
}
<<<<<<< HEAD
void newConnection1(int sockfd,const InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "Hellohello\n", 13);
         sockets::close(sockfd);
}
=======
>>>>>>> 70d2cf2efc6c59583ae0a97adb69af199046d1d0

int main()
{
  printf("main(): pid = %d\n", getpid());

  InetAddress listenAddr(9981);
<<<<<<< HEAD
  InetAddress listenAddr1(9980);
=======
>>>>>>> 70d2cf2efc6c59583ae0a97adb69af199046d1d0
  EventLoop loop;

  Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
<<<<<<< HEAD
  Acceptor acceptor1(&loop, listenAddr1);
  acceptor1.setNewConnectionCallback(newConnection1);
  acceptor1.listen();
=======
>>>>>>> 70d2cf2efc6c59583ae0a97adb69af199046d1d0

  loop.loop();
}
