#include "Server.h"
#include <iostream>
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  server->OnConnect([](Connection *conn) {
    conn->Recv();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      return;
    }
    std::cout << conn->GetReadBuffer()->ToStr() << std::endl;

    conn->SetSendBuffer(conn->GetReadBuffer()->ToStr());
    conn->Send();
  });

  loop->Loop();
  delete server;
  delete loop;
  return 0;
}
