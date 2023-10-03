.PHONY:all
all:shmclient shmserver

shmclient:shmclient.cc
	g++ -o $@ $^ -std=c++11
shmserver:shmserver.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf shmclient shmserver

	