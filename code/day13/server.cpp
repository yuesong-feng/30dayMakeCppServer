#include "src/include/Server.h"

#include "src/include/EventLoop.h"

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->Loop();
  return 0;
}
