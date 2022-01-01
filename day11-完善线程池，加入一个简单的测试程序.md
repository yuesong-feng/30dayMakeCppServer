# day11-完善线程池，加入一个简单的测试程序

在昨天的教程里，我们添加了一个最简单的线程池到服务器，一个完整的Reactor模式正式成型。这个线程池只是为了满足我们的需要构建出的最简单的线程池，存在很多问题。比如，由于任务队列的添加、取出都存在拷贝操作，线程池不会有太好的性能，只能用来学习，正确做法是使用右值移动、完美转发等阻止拷贝。另外线程池只能接受`std::function<void()>`类型的参数，所以函数参数需要事先使用`std::bind()`，并且无法得到返回值。

为了解决以上提到的问题，线程池的构造函数和析构函数都不会有太大变化，唯一需要改变的是将任务添加到任务队列的`add`函数。我们希望使用`add`函数前不需要手动绑定参数，而是直接传递，并且可以得到任务的返回值。新的实现代码如下：
```cpp
template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;  //返回值类型

    auto task = std::make_shared< std::packaged_task<return_type()> >(  //使用智能指针
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)  //完美转发参数
        );  
        
    std::future<return_type> res = task->get_future();  // 使用期约
    {   //队列锁作用域
        std::unique_lock<std::mutex> lock(tasks_mtx);   //加锁

        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });  //将任务添加到任务队列
    }
    cv.notify_one();    //通知一次条件变量
    return res;     //返回一个期约
}
```
这里使用了大量C++11之后的新标准，具体使用方法可以参考欧长坤《现代 C++ 教程》。另外这里使用了模版，所以不能放在cpp文件，因为C++编译器不支持模版的分离编译
> 这是一个复杂的问题，具体细节请参考《深入理解计算机系统》有关编译、链接的章节

此外，我们希望对现在的服务器进行多线程、高并发的测试，所以需要使用网络库写一个简单的多线程高并发测试程序，具体实现请参考源代码，使用方式如下：

```bash
./test -t 10000 -m 10 (-w 100)
# 10000个线程，每个线程回显10次，建立连接后等待100秒开始发送消息（可用于测试服务器能同时保持的最大连接数）。不指定w参数，则建立连接后开始马上发送消息。
```
注意Makefile文件也已重写，现在使用make只能编译服务器，客户端、测试程序的编译指令请参考Makefile文件，服务器程序编译后可以使用vscode调试。也可以使用gdb调试：
```bash
gdb server  #使用gdb调试
r           #执行
where / bt  #查看调用栈
```
今天还发现了之前版本的一个缺点：对于`Acceptor`，接受连接的处理时间较短、报文数据极小，并且一般不会有特别多的新连接在同一时间到达，所以`Acceptor`没有必要采用epoll ET模式，也没有必要用线程池。由于不会成为性能瓶颈，为了简单最好使用阻塞式socket，故今天的源代码中做了以下改变：
1. Acceptor socket fd（服务器监听socket）使用阻塞式
2. Acceptor使用LT模式，建立好连接后处理事件fd读写用ET模式
3. Acceptor建立连接不使用线程池，建立好连接后处理事件用线程池

至此，今天的教程已经结束了。使用测试程序来测试我们的服务器，可以发现并发轻松上万。这种设计架构最容易想到、也最容易实现，但有很多缺点，具体请参考陈硕《Linux多线程服务器编程》第三章，在明天的教程中将使用one loop per thread模式改写。

此外，多线程系统编程是一件极其复杂的事情，比此教程中的设计复杂得多，由于这是入门教程，故不会涉及到太多细节，作者也还没有水平讲好这个问题。但要想成为一名合格的C++程序员，高并发编程是必备技能，还需要年复一年地阅读大量书籍、进行大量实践。
> 路漫漫其修远兮，吾将上下而求索    ———屈原《离骚》

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day11](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day11)
