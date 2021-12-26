# day07-为我们的服务器添加一个Acceptor

在上一天，我们分离了服务器类和事件驱动类，将服务器逐渐开发成Reactor模式。至此，所有服务器逻辑（目前只有接受新连接和echo客户端发来的数据）都写在`Server`类里。但很显然，`Server`作为一个服务器类，应该更抽象、更通用，我们应该对服务器进行进一步的模块化。

仔细分析可发现，对于每一个事件，不管提供什么样的服务，首先需要做的事都是调用`accept()`函数接受这个TCP连接，然后将socket文件描述符添加到epoll。当这个IO口有事件发生的时候，再对此TCP连接提供相应的服务。
> 在这里务必先理解TCP的面向连接这一特性，在谢希仁《计算机网络》里有详细的讨论。

因此我们可以分离接受连接这一模块，添加一个`Acceptor`类，这个类有以下几个特点：
- 类存在于事件驱动`EventLoop`类中，也就是Reactor模式的main-Reactor
- 类中的socket fd就是服务器监听的socket fd，每一个Acceptor对应一个socket fd
- 这个类也通过一个独有的`Channel`负责分发到epoll，该Channel的事件处理函数`handleEvent()`会调用Acceptor中的接受连接函数来新建一个TCP连接

根据分析，Acceptor类定义如下：
```cpp
class Acceptor{
private:
    EventLoop *loop;
    Socket *sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
};
```
这样一来，新建连接的逻辑就在`Acceptor`类中。但逻辑上新socket建立后就和之前监听的服务器socket没有任何关系了，TCP连接和`Acceptor`一样，拥有以上提到的三个特点，这两个类之间应该是平行关系。所以新的TCP连接应该由`Server`类来创建并管理生命周期，而不是`Acceptor`。并且将这一部分代码放在`Server`类里也并没有打破服务器的通用性，因为对于所有的服务，都要使用`Acceptor`来建立连接。

为了实现这一设计，我们可以用两种方式：
1. 使用传统的虚类、虚函数来设计一个接口
2. C++11的特性：std::function、std::bind、右值引用、std::move等实现函数回调

虚函数使用起来比较繁琐，程序的可读性也不够清晰明朗，而std::function、std::bind等新标准的出现可以完全替代虚函数，所以本教程采用第二种方式。
> 关于虚函数，在《C++ Primer》第十五章第三节有详细讨论，而C++11后的新标准可以参考欧长坤《现代 C++ 教程》

首先我们需要在Acceptor中定义一个新建连接的回调函数：
```cpp
std::function<void(Socket*)> newConnectionCallback;
```
在新建连接时，只需要调用这个回调函数：
```cpp
void Acceptor::acceptConnection(){
    newConnectionCallback(sock);
}
```
而这个回调函数本身的实现在`Server`类中：
```cpp
void Server::newConnection(Socket *serv_sock){
    // 接受serv_sock上的客户端连接
}
```
> 在今天的代码中，Acceptor的Channel使用了ET模式，事实上使用LT模式更合适，将在之后修复

新建Acceptor时通过std::bind进行绑定:
```cpp
acceptor = new Acceptor(loop);
std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
acceptor->setNewConnectionCallback(cb);
```
这样一来，尽管我们抽象分离出了`Acceptor`，新建连接的工作任然由`Server`类来完成。
> 请确保清楚地知道为什么要这么做再进行之后的学习。

至此，今天的教程已经结束了。在今天，我们设计了服务器接受新连接的`Acceptor`类。测试方法和之前一样，使用`make`得到服务器和客户端程序并运行。虽然服务器功能已经好几天没有变化了，但每一天我们都在不断抽象、不断完善，从结构化、流程化的程序设计，到面向对象程序设计，再到面向设计模式的程序设计，逐渐学习服务器开发的思想与精髓。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day07](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day07)
