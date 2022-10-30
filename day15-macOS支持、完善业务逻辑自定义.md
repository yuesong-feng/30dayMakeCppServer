# day15-macOS支持、完善业务逻辑自定义

作为程序员，使用MacBook电脑作为开发机很常见，本质和Linux几乎没有区别。本教程的EventLoop中使用Linux系统支持的epoll，然而macOS里并没有epoll，取而代之的是FreeBSD的kqueue，功能和使用都和epoll很相似。Windows系统使用WSL可以完美编译运行源代码，但MacBook则需要Docker、云服务器、或是虚拟机，很麻烦。在今天，我们将支持使用kqueue作为`EventLoop`类的Poller，使网络库可以在macOS等FreeBSD系统上原生运行。

在网络库已有的类当中，`Socket`和`Epoll`类是最底层的、需要和操作系统打交道，而上一层的`EventLoop`类只是使用`Epoll`提供的接口，而不关心`Epoll`类的底层实现。所以在考虑支持不同的操作系统时，只应该改变最底层的`Epoll`类，而不需要改动上层的`EventLoop`类。至于分发`fd`的`Channel`类，可以自定义epoll和kqueue的读、写、ET模式等事件，在`Channel`类中只需要注册好我们自定义的事件，然后在`Poller`类中将事件注册到epoll或kqueue。
```cpp
const int Channel::READ_EVENT = 1;
const int Channel::WRITE_EVENT = 2;
const int Channel::ET = 4;
```
需要注意`Channel`的用户自定义事件必须是1、2、4、8、16等十进制数，因为在`Poller`中判断、更新事件时需要用到按位与、按位或等操作，这里实际上是将16位二进制数的每一位用作标志位。如果这里理解有困难，可以先学一遍《深入理解计算机系统（第三版）》.

在`Poller`类中使用宏定义的形式判断当前操作系统，从而使用不同的代码:
```cpp
#ifdef OS_LINUX
// linux平台的代码
#endif

#ifdef OS_MACOS
// FreeBSD平台的代码
#endif
```
操作系统宏在CMakeLists.txt中定义：
```
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Platform: macOS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_MACOS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Platform: Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOS_LINUX")
endif()
```
这样就可以在不同的操作系统使用不同的代码。如注册/更新`Channel`，在Linux系统下会编译以下代码：
```cpp
void Poller::UpdateChannel(Channel *ch) {
  int sockfd = ch->GetSocket()->GetFd();
  struct epoll_event ev {};
  ev.data.ptr = ch;
  if (ch->GetListenEvents() & Channel::READ_EVENT) {
    ev.events |= EPOLLIN | EPOLLPRI;
  }
  if (ch->GetListenEvents() & Channel::WRITE_EVENT) {
    ev.events |= EPOLLOUT;
  }
  if (ch->GetListenEvents() & Channel::ET) {
    ev.events |= EPOLLET;
  }
  if (!ch->GetExist()) {
    ErrorIf(epoll_ctl(fd_, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add error");
    ch->SetExist();
  } else {
    ErrorIf(epoll_ctl(fd_, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll modify error");
  }
}
```
而在macOS系统下会编译以下代码：
```cpp
void Poller::UpdateChannel(Channel *ch) {
  struct kevent ev[2];
  memset(ev, 0, sizeof(*ev) * 2);
  int n = 0;
  int fd = ch->GetSocket()->GetFd();
  int op = EV_ADD;
  if (ch->GetListenEvents() & ch->ET) {
    op |= EV_CLEAR;
  }
  if (ch->GetListenEvents() & ch->READ_EVENT) {
    EV_SET(&ev[n++], fd, EVFILT_READ, op, 0, 0, ch);
  }
  if (ch->GetListenEvents() & ch->WRITE_EVENT) {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, op, 0, 0, ch);
  }
  int r = kevent(fd_, ev, n, NULL, 0, NULL);
  ErrorIf(r == -1, "kqueue add event error");
}
```

在之前的教程中，我们使`Connection`类以`OnConnect`回调函数的方式初步支持了业务逻辑自定义，自定义的业务逻辑是从服务器端可读事件触发后开始进入，所以需要自己处理读取数据的逻辑。这显然不合理，怎样事件触发、读取数据、异常处理等流程应该是网络库提供的基本功能，用户只应当关注怎样处理业务即可，所以业务逻辑的进入点应该是服务器读取完客户端的所有数据之后。这是，客户端传来的请求在`Connection`类的读缓冲区里，我们只需要根据请求来分发、处理业务即可。

通过设置`OnMessage`回调函数来自定义自己的业务逻辑，在服务器完全接收到客户端的数据之后，该函数触发。以下是一个echo服务器的业务逻辑：

```cpp
server->OnMessage([](Connection *conn){
  std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
  if(conn->GetState() == Connection::State::Connected){
    conn->Send(conn->ReadBuffer());
  }
});
```

在进入该函数前，服务器已经完成了接受客户端数据并保存在读缓冲区里，业务逻辑只需要将读缓冲区里的数据发送回即可，这样的设计更加符合服务器的功能准则与设计准则。

在今天的教程中，我们支持了MacOS、FreeBSD平台。在代码中去掉了`Epoll`类，改为通用的`Poller`，在不同的平台会自适应地编译不同的代码。同时我们将`Channel`类和操作系统脱离开来，通过用户自定义事件来表示监听、发生的事件。现在在Linux和macOS系统中，网络库都可以原生编译运行。但具体功能可能会根据操作系统的不同有细微差异，如在macOS平台下的并发支持度明显没有Linux平台高，在后面的开发中会不断完善。我们也完善了业务逻辑自定义，进一步简化了服务器编程、隐藏了更多细节，使用者只需要完全关注自己核心的业务逻辑。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day15](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day15)