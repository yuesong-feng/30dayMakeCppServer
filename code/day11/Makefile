src=$(wildcard src/*.cpp)

server:
	g++ server.cpp \
	-pthread -g \
	$(src) \
	-o server
	
client:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp client.cpp -o client

threadTest:
	g++ -pthread src/ThreadPoll.cpp ThreadPollTest.cpp -o ThreadPollTest

test:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp src/ThreadPoll.cpp \
	-pthread \
	test.cpp -o test

clean:
	rm server && rm client && rm test