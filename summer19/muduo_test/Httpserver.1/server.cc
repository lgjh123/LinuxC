#include "src/http/HttpServer.h"
#include "src/http/HttpRequest.h"
#include "src/http/HttpResponse.h"
#include "src/net/EventLoop.h"


#include <iostream>
#include <map>



int main(int argc, char* argv[])
{
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000));
  server.setThreadNum(1);
  server.start();
  loop.loop();
}
