# day05-epoll高级用法-Channel登场

在上一天，我们已经完整地开发了一个echo服务器，并且引入面向对象编程的思想，初步封装了`Socket`、`InetAddress`和`Epoll`，大大精简了主程序，隐藏了底层语言实现细节、增加了可读性。

让我们来回顾一下我们是如何使用`epoll`：将一个文件描述符添加到`epoll`红黑树，当该文件描述符上有事件发生时，拿到它、处理事件，这样我们每次只能拿到一个文件描述符，也就是一个`int`类型的整型值。试想，如果一个服务器同时提供不同的服务，如HTTP、FTP等，那么就算文件描述符上发生的事件都是可读事件，不同的连接类型也将决定不同的处理逻辑，仅仅通过一个文件描述符来区分显然会很麻烦，我们更加希望拿到关于这个文件描述符更多的信息。

在day03介绍`epoll`时，曾讲过`epoll_event`结构体：
```cpp
typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
struct epoll_event {
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;
```
可以看到，epoll中的`data`其实是一个union类型，可以储存一个指针。而通过指针，理论上我们可以指向任何一个地址块的内容，可以是一个类的对象，这样就可以将一个文件描述符封装成一个`Channel`类，一个Channel类自始至终只负责一个文件描述符，对不同的服务、不同的事件类型，都可以在类中进行不同的处理，而不是仅仅拿到一个`int`类型的文件描述符。
> 这里读者务必先了解C++中的union类型，在《C++ Primer（第五版）》第十九章第六节有详细说明。

`Channel`类的核心成员如下：
```cpp
class Channel{
private:
    Epoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
};
```
显然每个文件描述符会被分发到一个`Epoll`类，用一个`ep`指针来指向。类中还有这个`Channel`负责的文件描述符。另外是两个事件变量，`events`表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。`revents`表示在`epoll`返回该`Channel`时文件描述符正在发生的事件。`inEpoll`表示当前`Channel`是否已经在`epoll`红黑树中，为了注册`Channel`的时候方便区分使用`EPOLL_CTL_ADD`还是`EPOLL_CTL_MOD`。

接下来以`Channel`的方式使用epoll：
新建一个`Channel`时，必须说明该`Channel`与哪个`epoll`和`fd`绑定：
```cpp
Channel *servChannel = new Channel(ep, serv_sock->getFd());
```
这时该`Channel`还没有被添加到epoll红黑树，因为`events`没有被设置，不会监听该`Channel`上的任何事件发生。如果我们希望监听该`Channel`上发生的读事件，需要调用一个`enableReading`函数：
```cpp
servChannel->enableReading();
```
调用这个函数后，如`Channel`不在epoll红黑树中，则添加，否则直接更新`Channel`、打开允许读事件。`enableReading`函数如下：
```cpp
void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    ep->updateChannel(this);
}
```
可以看到该函数做了两件事，将要监听的事件`events`设置为读事件并采用ET模式，然后在ep指针指向的Epoll红黑树中更新该`Channel`，`updateChannel`函数的实现如下：
```cpp
void Epoll::updateChannel(Channel *channel){
    int fd = channel->getFd();  //拿到Channel的文件描述符
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();   //拿到Channel希望监听的事件
    if(!channel->getInEpoll()){
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");//添加Channel中的fd到epoll
        channel->setInEpoll();
    } else{
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");//已存在，则修改
    }
}
```
在使用时，我们可以通过`Epoll`类中的`poll()`函数获取当前有事件发生的`Channel`：
```cpp
while(true){
    vector<Channel*> activeChannels = ep->poll();
    // activeChannels是所有有事件发生的Channel
}
```
注：在今天教程的源代码中，并没有将事件处理改为使用`Channel`回调函数的方式，仍然使用了之前对文件描述符进行处理的方法，这是错误的，将在明天的教程中进行改写。

至此，day05的主要教程已经结束了，完整源代码请在`code/day05`文件夹。服务器的功能和昨天一样，添加了`Channel`类，可以让我们更加方便简单、多样化地处理epoll中发生的事件。同时脱离了底层，将epoll、文件描述符和事件进行了抽象，形成了事件分发的模型，这也是Reactor模式的核心，将在明天的教程进行讲解。

进入`code/day05`文件夹，使用make命令编译，将会得到`server`和`client`，输入命令`./server`开始运行服务器。然后在一个新终端输入命令`./client`运行客户端，可以看到服务器接收到了客户端的连接请求，并成功连接。再新开一个或多个终端，运行client，可以看到这些客户端也同时连接到了服务器。此时我们在任意一个client输入一条信息，服务器都显示并发送到该客户端。如使用`control+c`终止掉某个client，服务器回显示这个client已经断开连接，但其他client并不受影响。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day05](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day05)
