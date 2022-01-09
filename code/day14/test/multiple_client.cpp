#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>

#include "Connection.h"
#include "Socket.h"
#include "ThreadPool.h"

void OneClient(int msgs, int wait) {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);
  Connection *conn = new Connection(nullptr, sock);
  sleep(wait);
  int count = 0;
  while (count < msgs) {
    conn->SetSendBuffer("I'm client!");
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "msg count " << count++ << ": " << conn->ReadBuffer() << std::endl;
  }
  delete conn;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o = -1;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = std::stoi(optarg);
        break;
      case 'm':
        msgs = std::stoi(optarg);
        break;
      case 'w':
        wait = std::stoi(optarg);
        break;
      case '?':
        printf("error optopt: %c\n", optopt);
        printf("error opterr: %d\n", opterr);
        break;
      default:
        break;
    }
  }

  ThreadPool *poll = new ThreadPool(threads);
  std::function<void()> func = std::bind(OneClient, msgs, wait);
  for (int i = 0; i < threads; ++i) {
    poll->Add(func);
  }
  delete poll;
  return 0;
}
