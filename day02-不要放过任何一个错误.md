# day02-不要放过任何一个错误
在上一天，我们写了一个客户端发起socket连接和一个服务器接受socket连接。然而对于`socket`,`bind`,`listen`,`accept`,`connect`等函数，我们都设想程序完美地、没有任何异常地运行，而这显然是不可能的，不管写代码水平多高，就算你是林纳斯，也会在程序里写出bug。

在《Effective C++》中条款08讲到，别让异常逃离析构函数。在这里我拓展一下，我们不应该放过每一个异常，否则在大型项目开发中一定会遇到很难定位的bug！
> 具体信息可以参考《Effective C++》原书条款08，这里不再赘述。

对于Linux系统调用，常见的错误提示方式是使用返回值和设置errno来说明错误类型。
> 详细的C++语言异常处理请参考《C++ Primer》第五版第五章第六节

通常来讲，当一个系统调用返回-1，说明有error发生。我们来看看socket编程最常见的错误处理模版：
```cpp
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if(sockfd == -1)
{
    print("socket create error");
    exit(-1);
}
```
为了处理一个错误，需要至少占用五行代码，这使编程十分繁琐，程序也不好看，异常处理所占篇幅比程序本身都多。

为了方便编码以及代码的可读性，可以封装一个错误处理函数：
```cpp
void errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
```
第一个参数是是否发生错误，如果为真，则表示有错误发生，会调用`<stdio.h>`头文件中的`perror`，这个函数会打印出`errno`的实际意义，还会打印出我们传入的字符串，也就是第函数第二个参数，让我们很方便定位到程序出现错误的地方。然后使用`<stdlib.h>`中的`exit`函数让程序退出并返回一个预定义常量`EXIT_FAILURE`。

在使用的时候:
```cpp
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
errif(sockfd == -1, "socket create error");
```
这样我们只需要使用一行进行错误处理，写起来方便简单，也输出了自定义信息，用于定位bug。

对于所有的函数，我们都使用这种方式处理错误：
```cpp
errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
errif(clnt_sockfd == -1, "socket accept error");
errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
```
到现在最简单的错误处理函数已经封装好了，但这仅仅用于本教程的开发，在真实的服务器开发中，错误绝不是一个如此简单的话题。

当我们建立一个socket连接后，就可以使用`<unistd.h>`头文件中`read`和`write`来进行网络接口的数据读写操作了。
> 这两个函数用于TCP连接。如果是UDP，需要使用`sendto`和`recvfrom`，这些函数的详细用法可以参考游双《Linux高性能服务器编程》第五章第八节。

接下来的教程用注释的形式写在代码中，先来看服务器代码：
```cpp
while (true) {
    char buf[1024];     //定义缓冲区
    bzero(&buf, sizeof(buf));       //清空缓冲区
    ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf)); //从客户端socket读到缓冲区，返回已读数据大小
    if(read_bytes > 0){
        printf("message from client fd %d: %s\n", clnt_sockfd, buf);  
        write(clnt_sockfd, buf, sizeof(buf));           //将相同的数据写回到客户端
    } else if(read_bytes == 0){             //read返回0，表示EOF
        printf("client fd %d disconnected\n", clnt_sockfd);
        close(clnt_sockfd);
        break;
    } else if(read_bytes == -1){        //read返回-1，表示发生错误，按照上文方法进行错误处理
        close(clnt_sockfd);
        errif(true, "socket read error");
    }
}
```
客户端代码逻辑是一样的：
```cpp
while(true){
    char buf[1024];     //定义缓冲区
    bzero(&buf, sizeof(buf));       //清空缓冲区
    scanf("%s", buf);             //从键盘输入要传到服务器的数据
    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));      //发送缓冲区中的数据到服务器socket，返回已发送数据大小
    if(write_bytes == -1){          //write返回-1，表示发生错误
        printf("socket already disconnected, can't write any more!\n");
        break;
    }
    bzero(&buf, sizeof(buf));       //清空缓冲区 
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));    //从服务器socket读到缓冲区，返回已读数据大小
    if(read_bytes > 0){
        printf("message from server: %s\n", buf);
    }else if(read_bytes == 0){      //read返回0，表示EOF，通常是服务器断开链接，等会儿进行测试
        printf("server socket disconnected!\n");
        break;
    }else if(read_bytes == -1){     //read返回-1，表示发生错误，按照上文方法进行错误处理
        close(sockfd);
        errif(true, "socket read error");
    }
}
```
> 一个小细节，Linux系统的文件描述符理论上是有限的，在使用完一个fd之后，需要使用头文件`<unistd.h>`中的`close`函数关闭。更多内核相关知识可以参考Robert Love《Linux内核设计与实现》的第三版。

至此，day02的主要教程已经结束了，完整源代码请在`code/day02`文件夹，接下来看看今天的学习成果以及测试我们的服务器！

进入`code/day02`文件夹，使用make命令编译，将会得到`server`和`client`。输入命令`./server`开始运行，直到`accept`函数，程序阻塞、等待客户端连接。然后在一个新终端输入命令`./client`运行客户端，可以看到服务器接收到了客户端的连接请求，并成功连接。现在客户端阻塞在`scanf`函数，等待我们键盘输入，我们可以输入一句话，然后回车。在服务器终端，我们可以看到:
```
message from client fd 4: Hello!
```
然后在客户端，也能接受到服务器的消息：
```
message from server: Hello!
```
> 由于是一个`while(true)`循环，客户端可以一直输入，服务器也会一直echo我们的消息。由于`scanf`函数的特性，输入的语句遇到空格时，会当成多行进行处理，我们可以试试。

接下来在客户端使用`control+c`终止程序，可以看到服务器也退出了程序并显示：
```
client fd 4 disconnected
```
再次运行两个程序，这次我们使用`control+c`终止掉服务器，再试图从客户端发送信息，可以看到客户端输出：
```
server socket disconnected!
```
至此，我们已经完整地开发了一个echo服务器，并且有最基本的错误处理！

但现在，我们的服务器只能处理一个客户端，我们可以试试两个客户端同时连接服务器，看程序将会如何运行。在day03的教程里，我们将会讲解Linux系统高并发的基石--epoll，并编程实现一个可以支持无数客户端同时连接的echo服务器！

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day02](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day02)
