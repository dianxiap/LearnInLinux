.PHONY:all
all:client server

client:CalClient.cc
	g++ -o $@ $^ -std=c++11
server:CalServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -rf client server