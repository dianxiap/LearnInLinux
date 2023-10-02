.PHONY:all
all:client mutiServer

client:client.cc
	g++ -o $@ $^ -std=c++11
mutiServer:server.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf client mutiServer

	