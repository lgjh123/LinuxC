#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "EventLoop.h"


#include <iostream>
#include <map>


//bool benchmark = false;

int main(int argc, char* argv[])
{
  int numThreads = 0;
  
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000));
  server.start();
  loop.loop();
}