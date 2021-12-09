# day11-完善线程池、完善服务器

代码已写好，重写了Makefile，make后支持vscode调试

完善了线程池，支持返回类型推导，修复了线程池拼写错误。同时完善之前的服务器，修复了以下bug

bug1：acceptor应该使用LT模式，建立好连接后处理事件用ET

bug2：acceptor建立连接不应该使用线程池，建立好连接后处理事件用线程池

bug3：线程池add函数不能放在cpp文件中，原因不明

使用线程池，并发太高断开连接经常会出现Segmentation fault或Bad file description等众多错误

当前模式缺点很多，需要改用one loop per thread模式重写

写了多线程测试程序：

./test -t 10000 -m 10 (-w 100)

10000个线程，每个线程回显10次，建立连接后等待100秒开始发送消息

gdb server

r

where / bt