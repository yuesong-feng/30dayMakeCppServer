#include "Server.h"
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include <signal.h>
#include <iostream>

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  server->OnConnect([](Connection *conn) {
    conn->Recv();
    if(conn == nullptr){
      printf("here\n");
    }
    conn->send_buffer_->SetBuf("hello client");
    conn->Send();
  });

  loop->Loop();
  delete server;
  delete loop;
  return 0;
}
