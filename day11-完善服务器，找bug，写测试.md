# day11-完善服务器，找bug，写测试

bug1：acceptor应该使用LT模式，建立好连接后处理事件用ET

bug2：acceptor建立连接不应该使用线程池，建立好连接后处理事件用线程池

bug3：使用线程池，并发太高断开连接经常会Segmentation fault


测试程序：

./test -t 10000 -m 10

10000个线程，每个线程回显10次

ulimit -a

ulimit -c unlimited

gdb --core=core

where / bt