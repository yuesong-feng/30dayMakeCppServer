#include "pine.h"
#include <iostream>

int main() {
  Socket *sock = new Socket();
  sock->Create();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(sock->fd(), nullptr);

  while (true) {
    std::string input;
    std::getline(std::cin, input);
    conn->set_send_buf(input.c_str());
    conn->Write();
    if (conn->state() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "Message from server: " << conn->read_buf()->c_str() << std::endl;
  }

  delete conn;
  delete sock;
  return 0;
}
