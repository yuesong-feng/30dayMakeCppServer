# day12-将服务器改写为主从Reactor多线程模式

在上一天的教程，我们实现了一种最容易想到的多线程Reactor模式，即将每一个Channel的任务分配给一个线程执行。这种模式有很多缺点，逻辑上也有不合理的地方。比如当前版本线程池对象被`EventLoop`所持有，这显然是不合理的，线程池显然应该由服务器类来管理，不应该和事件驱动产生任何关系。如果强行将线程池放进`Server`类中，由于`Channel`类只有`EventLoop`对象成员，使用线程池则需要注册回调函数，十分麻烦。
> 更多比较可以参考陈硕《Linux多线程服务器编程》第三章

今天我们将采用主从Reactor多线程模式，也是大多数高性能服务器采用的模式，即陈硕《Linux多线程服务器编程》书中的one loop per thread模式。

此模式的特点为：
1. 服务器一般只有一个main Reactor，有很多个sub Reactor。
2. 服务器管理一个线程池，每一个sub Reactor由一个线程来负责`Connection`上的事件循环，事件执行也在这个线程中完成。
3. main Reactor只负责`Acceptor`建立新连接，然后将这个连接分配给一个sub Reactor。

此时，服务器有如下成员：
```cpp
class Server {
private:
    EventLoop *mainReactor;     //只负责接受连接，然后分发给一个subReactor
    Acceptor *acceptor;                     //连接接受器
    std::map<int, Connection*> connections; //TCP连接
    std::vector<EventLoop*> subReactors;    //负责处理事件循环
    ThreadPool *thpool;     //线程池
};
```
在构造服务器时：
```cpp
Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr){ 
    acceptor = new Acceptor(mainReactor);   //Acceptor由且只由mainReactor负责
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();     //线程数量，也是subReactor数量
    thpool = new ThreadPool(size);      //新建线程池
    for(int i = 0; i < size; ++i){
        subReactors.push_back(new EventLoop());     //每一个线程是一个EventLoop
    }

    for(int i = 0; i < size; ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop);      //开启所有线程的事件循环
    }
}
```
在新连接到来时，我们需要将这个连接的socket描述符添加到一个subReactor中：
```cpp
int random = sock->getFd() % subReactors.size();    //调度策略：全随机
Connection *conn = new Connection(subReactors[random], sock);   //分配给一个subReactor
```
这里有一个很值得研究的问题：当新连接到来时应该分发给哪个subReactor，这会直接影响服务器效率和性能。这里采用了最简单的hash算法实现全随机调度，即将新连接随机分配给一个subReactor。由于socket fd是一个`int`类型的整数，只需要用fd余subReactor数，即可以实现全随机调度。

这种调度算法适用于每个socket上的任务处理时间基本相同，可以让每个线程均匀负载。但事实上，不同的业务传输的数据极有可能不一样，也可能受到网络条件等因素的影响，极有可能会造成一些subReactor线程十分繁忙，而另一些subReactor线程空空如也。此时需要使用更高级的调度算法，如根据繁忙度分配，或支持动态转移连接到另一个空闲subReactor等，读者可以尝试自己设计一种比较好的调度算法。

至此，今天的教程就结束了。在今天，一个简易服务器的所有核心模块已经开发完成，采用主从Reactor多线程模式。在这个模式中，服务器以事件驱动作为核心，服务器线程只负责mainReactor的新建连接任务，同时维护一个线程池，每一个线程也是一个事件循环，新连接建立后分发给一个subReactor开始事件监听，有事件发生则在当前线程处理。这种模式几乎是目前最先进、最好的服务器设计模式，本教程之后也会一直采用此模式。

虽然架构上已经完全开发完毕了，但现在我们还不算拥有一个完整的网络库，因为网络库的业务是写死的`echo`服务，十分单一，如果要提供其他服务，如HTTP服务、FTP服务等，需要重新开发、重新写代码，这打破了通用性原则。我们希望将服务器业务处理也进一步抽象，实现用户特例化，即在`main`函数新建`Server`的时候，可以自己设计、绑定相应的业务，在之后的教程将会实现这一功能。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day12](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day12)
