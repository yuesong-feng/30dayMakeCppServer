#include <Connection.h>
#include <Socket.h>
#include <iostream>

int main() {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(nullptr, sock);
  while(true){
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
  }
  // conn->Read();

//  if (conn->GetState() == Connection::State::Connected) {
  //  std::cout << conn->ReadBuffer() << std::endl;
  //}
  //conn->SetSendBuffer("Hello server!");
  //conn->Write();

  delete conn;
  return 0;
}
