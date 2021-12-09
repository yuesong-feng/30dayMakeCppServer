src=$(wildcard src/*.cpp)

server:
	g++ -std=c++11 -pthread -g \
	$(src) \
	server.cpp \
	-o server
	
client:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp client.cpp -o client

th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

test:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp src/ThreadPool.cpp \
	-pthread \
	test.cpp -o test

clean:
	rm server && rm client && rm test