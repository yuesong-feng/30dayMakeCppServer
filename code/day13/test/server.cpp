#include "Server.h"

#include "EventLoop.h"

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->Loop();
  delete server;
  delete loop;
  return 0;
}
