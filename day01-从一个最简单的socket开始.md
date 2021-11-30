# day01

如果读者之前有计算机网络的基础知识那就更好了，没有也没关系，socket编程非常容易上手。但本教程主要偏向实践，不会详细讲述计算机网络协议、网络编程原理等。想快速入门可以看以下博客，讲解比较清楚、错误较少：

- [计算机网络基础知识总结](https://www.runoob.com/w3cnote/summary-of-network.html)

要想打好基础，抄近道是不可的，有时间一定要认真学一遍谢希仁的《计算机网络》，要想精通服务器开发，这必不可少。

首先在服务器，我们需要建立一个socket套接字，对外提供一个网络通信接口，在Linux系统中这个套接字竟然仅仅是一个文件描述符，也就是一个`int`类型的值！

> 在这里读者务必先了解什么是Linux文件描述符，C语言中文网的这篇文章讲得很详细：[socket是什么？套接字是什么？](http://c.biancheng.net/view/2123.html)

> Unix哲学KISS：keep it simple, stupid。在Linux系统里，一切看上去十分复杂的逻辑功能，都用简单到不可思议的方式实现，甚至有些时候看上去很愚蠢。但仔细推敲，人们将会赞叹Linux的精巧设计，或许这就是大智若愚。

```c++
#include <sys/socket.h>
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```
- 第一个参数：AF_INET表示使用IPv4
- 第二个参数：协议，SOCK_STREAM表示使用TCP协议
- 第三个参数：0表示根据前面的两个参数自动推导协议类型

对于客户端，服务器存在的唯一标识是一个IP地址和端口，这时候我们需要将这个套接字绑定到一个IP地址和端口上

```c++
struct sockaddr_in addr;
bzero(&addr, sizeof(addr));


```