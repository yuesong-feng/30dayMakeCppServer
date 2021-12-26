# day08-一切皆是类，连TCP连接也不例外

在上一天，我们分离了用于接受连接的`Acceptor`类，并把新建连接的逻辑放在了`Server`类中。在上一天我们还提到了`Acceptor`类最主要的三个特点：
- 类存在于事件驱动`EventLoop`类中，也就是Reactor模式的main-Reactor
- 类中的socket fd就是服务器监听的socket fd，每一个Acceptor对应一个socket fd
- 这个类也通过一个独有的`Channel`负责分发到epoll，该Channel的事件处理函数`handleEvent()`会调用Acceptor中的接受连接函数来新建一个TCP连接

对于TCP协议，三次握手新建连接后，这个连接将会一直存在，直到我们四次挥手断开连接。因此，我们也可以把TCP连接抽象成一个`Connection`类，这个类也有以下几个特点：
- 类存在于事件驱动`EventLoop`类中，也就是Reactor模式的main-Reactor
- 类中的socket fd就是客户端的socket fd，每一个Connection对应一个socket fd
- 每一个类的实例通过一个独有的`Channel`负责分发到epoll，该Channel的事件处理函数`handleEvent()`会调用Connection中的事件处理函数来响应客户端请求

可以看到，`Connection`类和`Acceptor`类是平行关系、十分相似，他们都直接由`Server`管理，由一个`Channel`分发到epoll，通过回调函数处理相应事件。唯一的不同在于，`Acceptor`类的处理事件函数（也就是新建连接功能）被放到了`Server`类中，具体原因在上一天的教程中已经详细说明。而`Connection`类则没有必要这么做，处理事件的逻辑应该由`Connection`类本身来完成。

另外，一个高并发服务器一般只会有一个`Acceptor`用于接受连接（也可以有多个），但可能会同时拥有成千上万个TCP连接，也就是成千上万个`Connection`类的实例，我们需要把这些TCP连接都保存起来。现在我们可以改写服务器核心`Server`类，定义如下：
```cpp
class Server {
private:
    EventLoop *loop;    //事件循环
    Acceptor *acceptor; //用于接受TCP连接
    std::map<int, Connection*> connections; //所有TCP连接
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);  //处理客户端请求
    void newConnection(Socket *sock);   //新建TCP连接
    void deleteConnection(Socket *sock);   //断开TCP连接
};
```
在接受连接后，服务器把该TCP连接保存在一个`map`中，键为该连接客户端的socket fd，值为指向该连接的指针。该连接客户端的socket fd通过一个`Channel`类分发到epoll，该`Channel`的事件处理回调函数`handleEvent()`绑定为`Connection`的业务处理函数，这样每当该连接的socket fd上发生事件，就会通过`Channel`调用具体连接类的业务处理函数，伪代码如下：
```cpp
void Connection::echo(int sockfd){
    // 回显sockfd发来的数据
}
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr){
    channel = new Channel(loop, sock->getFd()); //该连接的Channel
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd()); 
    channel->setCallback(cb); //绑定回调函数
    channel->enableReading(); //打开读事件监听
}
```
对于断开TCP连接操作，也就是销毁一个`Connection`类的实例。由于`Connection`的生命周期由`Server`进行管理，所以也应该由`Server`来删除连接。如果在`Connection`业务中需要断开连接操作，也应该和之前一样使用回调函数来实现，在`Server`新建每一个连接时绑定删除该连接的回调函数：
```cpp
Connection *conn = new Connection(loop, sock);
std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
conn->setDeleteConnectionCallback(cb);  // 绑定删除连接的回调函数

void Server::deleteConnection(Socket * sock){
    // 删除连接
}
```
至此，今天的教程已经结束，我们将TCP连接抽象成一个类，服务器模型更加成型。测试方法和之前一样，使用`make`得到服务器和客户端程序并运行。

这个版本是一个比较重要的版本，服务器最核心的几个模块都已经抽象出来，Reactor事件驱动大体成型（除了线程池），各个类的生命周期也大体上合适了，一个完整的单线程服务器设计模式已经编码完成了，读者应该完全理解今天的服务器代码后再继续后面的学习。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day08](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day08)
