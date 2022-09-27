# day14-支持业务逻辑自定义、完善Connection类

回顾之前的教程，可以看到服务器Echo业务的逻辑在`Connection`类中。如果我们需要不同的业务逻辑，如搭建一个HTTP服务器，或是一个FTP服务器，则需要改动`Connection`中的代码，这显然是不合理的。`Connection`类作为网络库的一部分，不应该和业务逻辑产生联系，业务逻辑应该由网络库用户自定义，写在`server.cpp`中。同时，作为一个通用网络库，客户端也可以使用网络库来编写相应的业务逻辑。今天我们需要完善`Connection`类，支持业务逻辑自定义。

首先来看看我们希望如何自定义业务逻辑，这是一个echo服务器的完整代码：

```cpp
int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  server->OnConnect([](Connection *conn) {  // 业务逻辑
    conn->Read();
    std::cout << "Message from client " << conn->GetSocket()->GetFd() << ": " << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      return;
    }
    conn->SetSendBuffer(conn->ReadBuffer());
    conn->Write();
  });
  loop->Loop(); // 开始事件循环
  delete server;
  delete loop;
  return 0;
}
```

这里新建了一个服务器和事件循环，然后以回调函数的方式编写业务逻辑。通过`Server`类的`OnConnection`设置lambda回调函数，回调函数的参数是一个`Connection`指针，代表服务器到客户端的连接，在函数体中可以书写业务逻辑。这个函数最终会绑定到`Connection`类的`on_connect_callback_`，也就是`Channel`类处理的事件（这个版本只考虑了可读事件）。这样每次有事件发生，事件处理实际上都在执行用户在这里写的代码逻辑。

关于`Connection`类的使用，提供了两个函数，分别是`Write()`和`Read()`。`Write()`函数表示将`write_buffer_`里的内容发送到该`Connection`的socket，发送后会清空写缓冲区；而`Read()`函数表示清空`read_buffer_`，然后将TCP缓冲区内的数据读取到读缓冲区。

在业务逻辑中，`conn->Read()`表示从客户端读取数据到读缓冲区。在发送回客户端之前，客户端有可能会关闭连接，所以需要先判断`Connection`的状态是否为`Closed`。然后将写缓冲区设置为和读缓冲区一样的内容`conn->SetSendBuffer(conn->ReadBuffer())`，最后调用`conn->Write()`将写缓冲区的数据发送给客户端。

可以看到，现在`Connection`类只有从socket读写数据的逻辑，与具体业务没有任何关系，业务完全由用户自定义。

在客户端我们也希望使用网络库来写业务逻辑，首先来看看客户端的代码：

```cpp
int main() {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);
  Connection *conn = new Connection(nullptr, sock);
  while (true) {
    conn->GetlineSendBuffer();
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
  }
  delete conn;
  return 0;
}
```

注意这里和服务器有很大的不同，之前设计的`Connection`类显然不能满足要求，所以需要完善`Connection`。

首先，这里没有服务器和事件循环，仅仅使用了一个裸的`Connection`类来表示从客户端到服务器的连接。所以此时`Read()`表示从服务器读取到客户端，而`Write()`表示从客户端写入到服务器，和之前服务器的`Conneciont`类方向完全相反。这样`Connection`就可以同时表示Server->Client或者Client->Server的连接，不需要新建一个类来区分，大大提高了通用性和代码复用。

其次，客户端`Connection`没有绑定事件循环，所以将第一个参数设置为`nullptr`表示不使用事件循环，这时将不会有`Channel`类创建来分配到`EventLoop`，表示使用一个裸的`Connection`。因此业务逻辑也不用设置服务器回调函数，而是直接写在客户端代码中。

另外，虽然服务器到客户端（Server->Client）的连接都使用非阻塞式socket IO（为了搭配epoll ET模式），但客户端到服务器（Client->Server）的连接却不一定，很多业务都需要使用阻塞式socket IO，比如我们当前的echo客户端。之前`Connection`类的读写逻辑都是非阻塞式socket IO，在这个版本支持了非阻塞式读写，代码如下：

```cpp
void Connection::Read() {
  ASSERT(state_ == State::Connected, "connection state is disconnected!");
  read_buffer_->Clear();
  if (sock_->IsNonBlocking()) {
    ReadNonBlocking();
  } else {
    ReadBlocking();
  }
}
void Connection::Write() {
  ASSERT(state_ == State::Connected, "connection state is disconnected!");
  if (sock_->IsNonBlocking()) {
    WriteNonBlocking();
  } else {
    WriteBlocking();
  }
  send_buffer_->Clear();
}
```

ps.如果连接是从服务器到客户端，所有的读写都应采用非阻塞式IO，阻塞式读写是提供给客户端使用的。

至此，今天的教程已经结束了。教程里只会包含极小一部分内容，大量的工作都在代码里，请务必结合源代码阅读。在今天的教程中，我们完善了`Connection`类，将`Connection`类与业务逻辑完全分离，业务逻辑完全由用户自定义。至此，我们的网络库核心代码已经完全脱离了业务，成为一个真正意义上的网络库。今天我们也将`Connection`通用化，同时支持Server->Client和Client->Server，使其可以在客户端脱离`EventLoop`单独绑定socket使用，读写操作也都支持了阻塞式和非阻塞式两种模式。

到今天，本教程已经进行了一半，我们开发了一个真正意义上的网络库，使用这个网络库，只需要不到20行代码，就可以搭建一个echo服务器、客户端（完整程序在`test`目录）。但这只是一个最简单的玩具型网络库，需要做的工作还很多，在今后的教程里，我们会对这个网络库不断完善、不断提升性能，使其可以在生产环境中使用。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day14](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day14)
