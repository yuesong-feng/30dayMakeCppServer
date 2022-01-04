#include <unistd.h>
#include <cstring>

#include <iostream>

#include "Buffer.h"
#include "Socket.h"
#include "util.h"

int main() {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 1234);
  sock->Connect(addr);

  int sockfd = sock->GetFd();

  Buffer *send_buffer = new Buffer();
  Buffer *read_buffer = new Buffer();

  while (true) {
    send_buffer->Getline();
    ssize_t write_bytes = write(sockfd, send_buffer->ToStr(), send_buffer->Size());
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    int already_read = 0;
    char buf[1024];  // 这个buf大小无所谓
    while (true) {
      memset(&buf, 0, sizeof(buf));
      ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
      if (read_bytes > 0) {
        read_buffer->Append(buf, read_bytes);
        already_read += read_bytes;
      } else if (read_bytes == 0) {  // EOF
        printf("server disconnected!\n");
        exit(EXIT_SUCCESS);
      }
      if (already_read >= send_buffer->Size()) {
        printf("message from server: %s\n", read_buffer->ToStr());
        break;
      }
    }
    read_buffer->Clear();
  }
  delete addr;
  delete sock;
  delete read_buffer;
  delete send_buffer;
  return 0;
}
