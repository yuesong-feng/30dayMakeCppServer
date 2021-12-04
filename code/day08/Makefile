server:
	g++ src/util.cpp client.cpp -o client && \
	g++ server.cpp \
	src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Connection.cpp \
	src/Channel.cpp src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp \
	-o server
clean:
	rm server && rm client