#include <unistd.h>
#include <cstring>

#include <iostream>

#include "Buffer.h"
#include "Connection.h"
#include "Socket.h"
#include "util.h"

int main() {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 1234);
  sock->Connect(addr);

  Connection *conn = new Connection(nullptr, sock);

  while (true) {
    conn->GetSendBuffer()->Getline();
    conn->Send();
    conn->Recv();
    std::cout << conn->GetReadBuffer()->ToStr() << std::endl;
    conn->GetReadBuffer()->Clear();
  }

  delete addr;
  delete sock;
  delete conn;
  return 0;
}
