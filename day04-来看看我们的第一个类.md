# day04-来看看我们的第一个类

在上一天，我们开发了一个支持多个客户端连接的服务器，但到目前为止，虽然我们的程序以`.cpp`结尾，本质上我们写的仍然是C语言程序。虽然C++语言完全兼容C语言并且大部分程序中都是混用，但一个很好的习惯是把C和C++看作两种语言，写代码时需要清楚地知道自己在写C还是C++。

另一点是我们的程序会变得越来越长、越来越庞大，虽然现在才不到100行代码，但把所有逻辑放在一个程序里显然是一种错误的做法，我们需要对程序进行模块化，每一个模块专门处理一个任务，这样可以增加程序的可读性，也可以写出更大庞大、功能更加复杂的程序。不仅如此，还可以很方便地进行代码复用，也就是造轮子。

C++是一门面向对象的语言，最低级的模块化的方式就是构建一个类。举个例子，我们的程序有新建服务器socket、绑定IP地址、监听、接受客户端连接等任务，代码如下：
```cpp
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
errif(sockfd == -1, "socket create error");

struct sockaddr_in serv_addr;
bzero(&serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
serv_addr.sin_port = htons(8888);

errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

struct sockaddr_in clnt_addr;
bzero(&clnt_addr, sizeof(clnt_addr));
socklen_t clnt_addr_len = sizeof(clnt_addr);

int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
errif(clnt_sockfd == -1, "socket accept error");
```
可以看到代码有19行，这已经是使用socket最精简的代码。在服务器开发中，我们或许会建立多个socket口，或许会处理多个客户端连接，但我们并不希望每次都重复编写这么多行代码，我们希望这样使用：
```cpp
Socket *serv_sock = new Socket();
InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
serv_sock->bind(serv_addr);
serv_sock->listen();   
InetAddress *clnt_addr = new InetAddress();  
Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));    
```
仅仅六行代码就可以实现和之前一样的功能，这样的使用方式忽略了底层的语言细节，不用在程序中考虑错误处理，更简单、更加专注于程序的自然逻辑，大家毫无疑问也肯定希望以这样简单的方式使用socket。

类似的还有epoll，最精简的使用方式为：
```cpp
int epfd = epoll_create1(0);
errif(epfd == -1, "epoll create error");

struct epoll_event events[MAX_EVENTS], ev;
bzero(&events, sizeof(events) * MAX_EVENTS);

bzero(&ev, sizeof(ev));
ev.data.fd = sockfd;
ev.events = EPOLLIN | EPOLLET;

epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

while(true){
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++i){
        // handle event
    }
}
```
而我们更希望这样来使用：
```cpp
Epoll *ep = new Epoll();
ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
while(true){
    vector<epoll_event> events = ep->poll();
    for(int i = 0; i < events.size(); ++i){
        // handle event
    }
}
```
同样完全忽略了如错误处理之类的底层细节，大大简化了编程，增加了程序的可读性。

在今天的代码中，程序的功能和昨天一样，仅仅将`Socket`、`InetAddress`和`Epoll`封装成类，这也是面向对象编程的最核心、最基本的思想。现在我们的目录结构为：
```
client.cpp
Epoll.cpp
Epoll.h
InetAddress.cpp
InetAddress.h
Makefile
server.cpp
Socket.cpp
Socket.h
util.cpp
util.h
```
注意在编译程序的使用，需要编译`Socket`、`InetAddress`和`Epoll`类的`.cpp`文件，然后进行链接，因为`.h`文件里只是类的定义，并未实现。
> C/C++程序编译、链接是一个很复杂的事情，具体原理请参考《深入理解计算机系统（第三版）》第七章。

至此，day04的主要教程已经结束了，完整源代码请在`code/day04`文件夹，服务器的功能和昨天一样。

进入`code/day04`文件夹，使用make命令编译，将会得到`server`和`client`，输入命令`./server`开始运行服务器。然后在一个新终端输入命令`./client`运行客户端，可以看到服务器接收到了客户端的连接请求，并成功连接。再新开一个或多个终端，运行client，可以看到这些客户端也同时连接到了服务器。此时我们在任意一个client输入一条信息，服务器都显示并发送到该客户端。如使用`control+c`终止掉某个client，服务器回显示这个client已经断开连接，但其他client并不受影响。

至此，我们已经完整地开发了一个echo服务器，并且引入面向对象编程的思想，初步封装了`Socket`、`InetAddress`和`Epoll`，大大精简了主程序，隐藏了底层语言实现细节、增加了可读性。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day04](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day04)
