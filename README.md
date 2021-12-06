# 30天自制C++服务器

先说结论：不管使用什么语言，一切后台开发的根基，是面向Linux的C/C++服务器开发。

几乎所有高并发服务器都是运行在Linux环境的，笔者之前也用Java、node写过服务器，但最后发现只是学会了一门技术、一门语言，而并不了解底层的基础原理。一个HTTP请求的过程，为什么可以实现高并发，如何控制TCP连接，如何处理好数据传输的逻辑等等，这些只有面向C/C++编程才能深入了解。

本教程模仿《30天自制操作系统》，面向零经验的新手，教你在30天内入门Linux服务器开发。本教程更偏向实践，将会把重点放在如何写代码上，而不会花太多的篇幅讲解背后的计算机基础原理，涉及到的地方会给出相应书籍的具体章节，但这并不代表这些理论知识不重要，事实上理论基础相当重要，没有理论的支撑，构建出一个高性能服务器是无稽之谈。

本教程希望读者：
- 熟悉C/C++语言
- 熟悉计算机网络基础，如TCP协议、socket原理等
- 了解基本的操作系统基础概念，如进程、线程、内存资源、系统调用等

学完本教程后，你将会很轻松地看懂muduo源码。

C/C++学习的一个难点在于初学时无法做出实际上的东西，没有反馈，程序都在黑乎乎的命令行里运行，不像web开发，可以随时看到自己学习的成果。本教程的代码都放在code文件夹里，每一天学习后都可以得到一个可以编译运行的服务器，不断迭代开发。

在code文件夹里有每一天的代码文件夹，进入该文件夹，使用`make`命令编译，会生成两个可执行文件，输入命令`./server`就能看到今天的学习成果！然后新建一个Terminal，然后输入`./client`运行客户端，与服务器交互。

[day01-从一个最简单的socket开始](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day01-从一个最简单的socket开始.md)

[day02-不要放过任何一个错误](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day02-不要放过任何一个错误.md)

[day03-高并发还得用epoll](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day03-高并发还得用epoll.md)

[day04-来看看我们的第一个类](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day04-来看看我们的第一个类.md)

[day05-epoll高级用法-Channel登场](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day05-epoll高级用法-Channel登场.md)

[day06-服务器与事件驱动核心类登场](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day06-服务器与事件驱动核心类登场.md)

[day07-为我们的服务器添加一个Acceptor](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day07-为我们的服务器添加一个Acceptor.md)

[day08-一切皆是类，连TCP连接也不例外](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day08-一切皆是类，连TCP连接也不例外.md)

[day09-缓冲区-大作用](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day09-缓冲区-大作用.md)

[day10-加入线程池到服务器](https://github.com/yuesong-feng/30dayMakeCppServer/blob/main/day10-加入线程池到服务器.md)

### todo list:

定时器

日志系统

拷贝控制

智能指针

性能优化-移动语义

多线程多工作线程模式Reactor

HTTP协议支持

用户特例化

webbench测试

使用CMake

......