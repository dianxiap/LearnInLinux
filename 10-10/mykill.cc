#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

void Usage(char* proc)
{
	printf("Usage: %s pid signo\n", proc);
}
int main(int argc, char* argv[])
{
	if (argc != 3){
		Usage(argv[0]);
		return 1;
	}
	pid_t pid = atoi(argv[1]);
	int signo = atoi(argv[2]);
	kill(pid, signo);
	return 0;
}
