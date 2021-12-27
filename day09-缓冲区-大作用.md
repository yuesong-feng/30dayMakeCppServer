# day09-缓冲区-大作用

在之前的教程中，一个完整的单线程服务器设计模式已经编码完成了。在进入多线程编程之前，应该完全理解单线程服务器的工作原理，因为多线程更加复杂、更加困难，开发难度远大于之前的单线程模式。不仅如此，读者也应根据自己的理解进行二次开发，完善服务器，比如非阻塞式socket模块就值得细细研究。

今天的教程和之前几天的不同，引入了一个最简单、最基本的的缓冲区，可以看作一个完善、改进服务器的例子，更加偏向于细节而不是架构。除了这一细节，读者也可以按照自己的理解完善服务器。

同时，我们已经封装了socket、epoll等基础组件，这些组件都可以复用。现在我们完全可以使用这个网络库来改写客户端程序，让程序更加简单明了，读者可以自己尝试用这些组件写一个客户端，然后和源代码中的对照。

在没有缓冲区的时候，服务器回送客户端消息的代码如下：
```cpp
#define READ_BUFFER 1024
void Connection::echo(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));   // 发送给客户端
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}
```
这是非阻塞式socket IO的读取，可以看到使用的读缓冲区大小为1024，每次从TCP缓冲区读取1024大小的数据到读缓冲区，然后发送给客户端。这是最底层C语言的编码，在逻辑上有很多不合适的地方。比如我们不知道客户端信息的真正大小是多少，只能以1024的读缓冲区去读TCP缓冲区（就算TCP缓冲区的数据没有1024，也会把后面的用空值补满）；也不能一次性读取所有客户端数据，再统一发给客户端。
> 关于TCP缓冲区、socket IO读取的细节，在《UNIX网络编程》卷一中有详细说明，想要精通网络编程几乎是必看的

虽然以上提到的缺点以C语言编程的方式都可以解决，但我们仍然希望以一种更加优美的方式读写socket上的数据，和其他模块一样，脱离底层，让我们使用的时候不用在意太多底层细节。所以封装一个缓冲区是很有必要的，为每一个`Connection`类分配一个读缓冲区和写缓冲区，从客户端读取来的数据都存放在读缓冲区里，这样`Connection`类就不再直接使用`char buf[]`这种最笨的缓冲区来处理读写操作。

缓冲区类的定义如下：
```cpp
class Buffer {
private:
    std::string buf;
public:
    void append(const char* _str, int _size);
    ssize_t size();
    const char* c_str();
    void clear();
    ......
};
```
> 这个缓冲区类使用`std::string`来储存数据，也可以使用`std::vector<char>`，有兴趣可以比较一下这两者的性能。

为每一个TCP连接分配一个读缓冲区后，就可以把客户端的信息读取到这个缓冲区内，缓冲区大小就是客户端发送的报文真实大小，代码如下：
```cpp
void Connection::echo(int sockfd){
    char buf[1024];     //这个buf大小无所谓
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}
```
在这里依然有一个`char buf[]`缓冲区，用于系统调用`read()`的读取，这个缓冲区大小无所谓，但太大或太小都可能对性能有影响（太小读取次数增多，太大资源浪费、单次读取速度慢），设置为1到设备TCP缓冲区的大小都可以。以上代码会把socket IO上的可读数据全部读取到缓冲区，缓冲区大小就等于客户端发送的数据大小。全部读取完成之后，可以构造一个写缓冲区、填好数据发送给客户端。由于是echo服务器，所以这里使用了相同的缓冲区。

至此，今天的教程已经结束，这个缓冲区只是为了满足当前的服务器功能而构造的一个最简单的`Buffer`类，还需要进一步完善，读者可以按照自己的方式构建缓冲区类，完善其他细节，为后续的多线程服务器做准备。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day09](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day09)
