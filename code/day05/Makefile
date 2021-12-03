server:
	g++ util.cpp client.cpp -o client && \
	g++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp Channel.cpp -o server
clean:
	rm server && rm client