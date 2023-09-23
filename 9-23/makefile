.PHONY:all
all:libtest.so 

libtest.so:add.o print.o 
	gcc -shared print.o add.o -o libtest.so
libtest.a: add.o print.o
	ar -rc libtest.a add.o print.o
add.o:add.c
	gcc -c -fPIC add.c -o  add.o
print.o:print.c
	gcc -c -fPIC print.c -o print.o

.PHONY:output
output:
	mkdir -p mylib/lib 
	mkdir -p mylib/include 
	cp -rf *.h mylib/include
	cp -rf *.so mylib/lib 

.PHONY:clean
clean:
	rm -rf *.o mylib *.a *.so output
