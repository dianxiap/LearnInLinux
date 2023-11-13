.PHONY:all
all:UdpClient UdpServer

UdpClient:UdpClient.cc
	g++ -o $@ $^ -std=c++11 -pthread
UdpServer:UdpServer.cc
	g++ -o $@ $^ -std=c++11 -pthread


.PHONY:clean
clean:
	rm -rf UdpClient UdpServer