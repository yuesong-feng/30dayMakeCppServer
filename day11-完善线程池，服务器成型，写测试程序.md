# day11-完善线程池、完善服务器

代码已写好

完善了线程池，支持返回类型推导，修复了线程池拼写错误

完善之前的服务器，修复了以下bug

bug1：acceptor应该使用LT模式，建立好连接后处理事件用ET

bug2：acceptor建立连接不应该使用线程池，建立好连接后处理事件用线程池

bug3：使用线程池，并发太高断开连接经常会Segmentation fault，原因不明

bug4：线程池add函数不能放在cpp文件中，原因不明

写了多线程测试程序：

./test -t 10000 -m 10

10000个线程，每个线程回显10次

ulimit -a

ulimit -c unlimited

gdb --core=core

where / bt