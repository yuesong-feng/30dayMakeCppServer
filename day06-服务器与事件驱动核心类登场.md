# day06-服务器与事件驱动核心类登场

在上一天，我们为每一个添加到epoll的文件描述符都添加了一个`Channel`，用户可以自由注册各种事件、很方便地根据不同事件类型设置不同回调函数（在当前的源代码中只支持了目前所需的可读事件，将在之后逐渐进行完善）。我们的服务器已经基本成型，但目前从新建socket、接受客户端连接到处理客户端事件，整个程序结构是顺序化、流程化的，我们甚至可以使用一个单一的流程图来表示整个程序。而流程化程序设计的缺点之一是不够抽象，当我们的服务器结构越来越庞大、功能越来越复杂、模块越来越多，这种顺序程序设计的思想显然是不能满足需求的。

对于服务器开发，我们需要用到更抽象的设计模式。从代码中我们可以看到，不管是接受客户端连接还是处理客户端事件，都是围绕epoll来编程，可以说epoll是整个程序的核心，服务器做的事情就是监听epoll上的事件，然后对不同事件类型进行不同的处理。这种以事件为核心的模式又叫事件驱动，事实上几乎所有的现代服务器都是事件驱动的。和传统的请求驱动模型有很大不同，事件的捕获、通信、处理和持久保留是解决方案的核心结构。libevent就是一个著名的C语言事件驱动库。

需要注意的是，事件驱动不是服务器开发的专利。事件驱动是一种设计应用的思想、开发模式，而服务器是根据客户端的不同请求提供不同的服务的一个实体应用，服务器开发可以采用事件驱动模型、也可以不采用。事件驱动模型也可以在服务器之外的其他类型应用中出现，如进程通信、k8s调度、V8引擎、Node.js等。

理解了以上的概念，就能容易理解服务器开发的两种经典模式——Reactor和Proactor模式。详细请参考游双《Linux高性能服务器编程》第八章第四节、陈硕《Linux多线程服务器编程》第六章第六节。

> 如何深刻理解Reactor和Proactor？ - 小林coding的回答 - 知乎
https://www.zhihu.com/question/26943938/answer/1856426252

由于Linux内核系统调用的设计更加符合Reactor模式，所以绝大部分高性能服务器都采用Reactor模式进行开发，我们的服务器也使用这种模式。

接下来我们要将服务器改造成Reactor模式。首先我们将整个服务器抽象成一个`Server`类，这个类中有一个main-Reactor（在这个版本没有sub-Reactor），里面的核心是一个`EventLoop`（libevent中叫做EventBase），这是一个事件循环，我们添加需要监听的事务到这个事件循环内，每次有事件发生时就会通知（在程序中返回给我们`Channel`），然后根据不同的描述符、事件类型进行处理（以回调函数的方式）。
> 如果你不太清楚这个自然段在讲什么，请先看一看前面提到的两本书的具体章节。

EventLoop类的定义如下：
```cpp
class EventLoop {
private:
    Epoll *ep;
    bool quit;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel*);
};
```
调用`loop()`函数可以开始事件驱动，实际上就是原来的程序中调用`epoll_wait()`函数的死循环：
```cpp
void EventLoop::loop(){
    while(!quit){
    std::vector<Channel*> chs;
        chs = ep->poll();
        for(auto it = chs.begin(); it != chs.end(); ++it){
            (*it)->handleEvent();
        }
    }
}
```
现在我们可以以这种方式来启动服务器，和muduo的代码已经很接近了：
```cpp
EventLoop *loop = new EventLoop();
Server *server = new Server(loop);
loop->loop();
```
服务器定义如下：
```cpp
class Server {
private:
    EventLoop *loop;
public:
    Server(EventLoop*);
    ~Server();
    void handleReadEvent(int);
    void newConnection(Socket *serv_sock);
};
```
这个版本服务器内只有一个`EventLoop`，当其中有可读事件发生时，我们可以拿到该描述符对应的`Channel`。在新建`Channel`时，根据`Channel`描述符的不同分别绑定了两个回调函数，`newConnection()`函数被绑定到服务器socket上，`handlrReadEvent()`被绑定到新接受的客户端socket上。这样如果服务器socket有可读事件，`Channel`里的`handleEvent()`函数实际上会调用`Server`类的`newConnection()`新建连接。如果客户端socket有可读事件，`Channel`里的`handleEvent()`函数实际上会调用`Server`类的`handlrReadEvent()`响应客户端请求。

至此，我们已经抽象出了`EventLoop`和`Channel`，构成了事件驱动模型。这两个类和服务器核心`Server`已经没有任何关系，经过完善后可以被任何程序复用，达到了事件驱动的设计思想，现在我们的服务器也可以看成一个最简易的Reactor模式服务器。

当然，这个Reactor模式并不是一个完整的Reactor模式，如处理事件请求仍然在事件驱动的线程里，这显然违背了Reactor的概念。我们还需要做很多工作，在接下来几天的教程里会进一步完善。


完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day06](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day06)
