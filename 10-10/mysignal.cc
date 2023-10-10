// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// int main()
// {
// 	int fd[2] = { 0 };
// 	if (pipe(fd) < 0){ //使用pipe创建匿名管道
// 		perror("pipe");
// 		return 1;
// 	}
// 	pid_t id = fork(); //使用fork创建子进程
// 	if (id == 0){
// 		//child
// 		close(fd[0]); //子进程关闭读端
// 		//子进程向管道写入数据
// 		const char* msg = "hello father, I am child...";
// 		int count = 10;
// 		while (count--){
// 			write(fd[1], msg, strlen(msg));
// 			sleep(1);
// 		}
// 		close(fd[1]); //子进程写入完毕，关闭文件
// 		exit(0);
// 	}
// 	//father
// 	close(fd[1]); //父进程关闭写端
// 	close(fd[0]); //父进程直接关闭读端（导致子进程被操作系统杀掉）
// 	int status = 0;
// 	waitpid(id, &status, 0);
// 	printf("child get signal:%d\n", status & 0x7F); //打印子进程收到的信号
// 	return 0;
// }

// #include <stdio.h>
// #include <signal.h>
// #include <unistd.h>

// int main()
// {
// 	int count = 0;
// 	alarm(1);
// 	while (1){
// 		count++;
// 		printf("count: %d\n", count);
// 	}
// 	return 0;
// }
// #include <stdio.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <unistd.h>

// int count = 0;
// void handler(int signo)
// {
// 	printf("get a signal: %d\n", signo);
// 	printf("count: %d\n", count);
// 	exit(1);
// }
// int main()
// {
// 	signal(SIGALRM, handler);
// 	alarm(1);
// 	while (1){
// 		count++;
// 	}
// 	return 0;
// }
// #include <iostream>
// #include <unistd.h>

// using namespace std;

// int main()
// {
//     cout<<"i am running ...\n";
//     sleep(3);
//     int* p=nullptr;
//     *p=100;
//     return 0;
// }
// #include <stdio.h>
// #include <unistd.h>
// #include <signal.h>

// void printPending(sigset_t *pending)
// {
// 	int i = 1;
// 	for (i = 1; i <= 31; i++){
// 		if (sigismember(pending, i)){
// 			printf("1");
// 		}
// 		else{
// 			printf("0");
// 		}
// 	}
// 	printf("\n");
// }
// int main()
// {
// 	sigset_t set, oset;
// 	sigemptyset(&set);
// 	sigemptyset(&oset);

// 	sigaddset(&set, 2); //SIGINT
// 	sigprocmask(SIG_SETMASK, &set, &oset); //阻塞2号信号

// 	sigset_t pending;
// 	sigemptyset(&pending);

// 	while (1){
// 		sigpending(&pending); //获取pending
// 		printPending(&pending); //打印pending位图（1表示未决）
// 		sleep(1);
// 	}
// 	return 0;
// }
// #include <stdio.h>
// #include <unistd.h>
// #include <signal.h>

// void printPending(sigset_t *pending)
// {
// 	int i = 1;
// 	for (i = 1; i <= 31; i++){
// 		if (sigismember(pending, i)){
// 			printf("1");
// 		}
// 		else{
// 			printf("0");
// 		}
// 	}
// 	printf("\n");
// }
// void handler(int signo)
// {
// 	printf("handler signo:%d\n", signo);
// }
// int main()
// {
// 	signal(2, handler);
// 	sigset_t set, oset;
// 	sigemptyset(&set);
// 	sigemptyset(&oset);

// 	sigaddset(&set, 2); //SIGINT
// 	sigprocmask(SIG_SETMASK, &set, &oset); //阻塞2号信号

// 	sigset_t pending;
// 	sigemptyset(&pending);

// 	int count = 0;
// 	while (1){
// 		sigpending(&pending); //获取pending
// 		printPending(&pending); //打印pending位图（1表示未决）
// 		sleep(1);
// 		count++;
// 		if (count == 20){
// 			sigprocmask(SIG_SETMASK, &oset, NULL); //恢复曾经的信号屏蔽字
// 			printf("恢复信号屏蔽字\n");
// 		}
// 	}
// 	return 0;
// }
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>

// struct sigaction act, oact;
// void handler(int signo)
// {
// 	printf("get a signal:%d\n", signo);
// 	sigaction(2, &oact, NULL);
// }
// int main()
// {
// 	memset(&act, 0, sizeof(act));
// 	memset(&oact, 0, sizeof(oact));

// 	act.sa_handler = handler;
// 	act.sa_flags = 0;
// 	sigemptyset(&act.sa_mask);

// 	sigaction(2, &act, &oact);
// 	while (1){
// 		printf("I am a process...\n");
// 		sleep(1);
// 	}
// 	return 0;
// }

//sigchld
// #include <iostream>
// #include <unistd.h>
// #include <signal.h>

// using namespace std;

// 如果不想关心子进程，并且我们还想让子进程退出之后，自动释放僵尸子进程
// int main()
// {
//     signal(SIGCHLD,SIG_IGN); //手动设置对子进程进行忽略
//     if(fork()==0)
//     {
//         cout<<"child: "<<getpid()<<endl;
//         sleep(5);
//         exit(0);
//     }

//     while(1)
//     {
//         cout<<"parent: "<<getpid()<<endl;
//         sleep(1);
//     }
// }

// void handler(int signum)
// {
//     cout<<"子进程退出： "<<signum<<" father pid: "<<getpid()<<endl;
// }
// int main()
// {
//     signal(SIGCHLD,handler);
//     if(fork()==0)
//     {
//         cout<<"child pid: "<<getpid()<<endl;
//         sleep(1);
//         exit(0);
//     }
//     while(1)sleep(1);
// }
// #include <stdio.h>
// #include <signal.h>

// volatile int flag = 0;

// void handler(int signo)
// {
// 	printf("get a signal:%d\n", signo);
// 	flag = 1;
// }
// int main()
// {
// 	signal(2, handler);
// 	while (!flag);
// 	printf("Proc Normal Quit!\n");
// 	return 0;
// }

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

void handler(int signo)
{
	printf("get a signal: %d\n", signo);
	int ret = 0;
	while ((ret = waitpid(-1, NULL, WNOHANG)) > 0){
		printf("wait child %d success\n", ret);
	}
}
int main()
{
	signal(SIGCHLD, handler);
	if (fork() == 0){
		//child
		printf("child is running, begin dead: %d\n", getpid());
		sleep(3);
		exit(1);
	}
	//father
	while (1);
	return 0;
}
