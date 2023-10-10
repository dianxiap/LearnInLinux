.PHONY:all
all: mykill mysignal

mykill:mykill.cc
	g++ -o $@ $^ -std=c++11 -g
mysignal:mysignal.cc
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm -rf mysignal	mykill