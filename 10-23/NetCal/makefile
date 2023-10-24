.PHONY:all
all:client CalServer

client:CalClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
CalServer:CalServer.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -f client CalServer