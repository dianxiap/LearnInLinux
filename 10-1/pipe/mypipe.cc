// #include <iostream>
// #include <cstdio>
// #include <cstring>
// #include <unistd.h>
// #include <assert.h>
// #include <string>
// #include <sys/wait.h>
// #include <sys/types.h>

// using namespace std;

// // 为什么不定义全局buffer？因为有写时拷贝！

// int main()
// {
//     // 1.创建管道
//     int pipefd[2] = {0}; // pipefd[0]: 读端，pipefd[1]: 写端
//     int n = pipe(pipefd);
//     // release模式下assert无效
//     assert(n != -1);
//     // 所以在n在release只被定义，没有被使用，在release模式下n会出下大量警告
//     (void)n;

// #ifdef DEBUG
//     cout << "pipefd[0]: " << pipefd[0] << endl;
//     cout << "pipefd[1]: " << pipefd[1] << endl;
// #endif

//     // 2.创建子进程
//     pid_t id = fork();
//     assert(id != -1);
//     if (id == 0)
//     {
//         // 子进程
//         // 3.构建单向通信的信道，父进程写入，子进程读取
//         // 3.1 关闭子进程不需要的fd
//         close(pipefd[1]);
//         char buffer[1024];
//         while (true)
//         {
//             ssize_t s = read(pipefd[0], buffer, sizeof(buffer) - 1);
//             if (s > 0)
//             {
//                 buffer[s] = 0;
//                 cout << "child get a message[" << getpid() << "] Father said# " << buffer << endl;
//             }
//         }
//         exit(0);
//     }
//     // 父进程
//     // 3.构建单向通信的信道
//     // 3.1 关闭父进程不需要的fd
//     close(pipefd[0]);
//     string message = "我是父进程，我正在给子进程发消息";
//     int count = 0;
//     char send_buffer[1024];
//     while (true)
//     {
//         // 3.2 构建一个变化的字符串
//         snprintf(send_buffer, sizeof(send_buffer), "%s[%d] : %d", message.c_str(), getpid(), count++);
//         // 3.3 写入
//         write(pipefd[1], send_buffer, strlen(send_buffer));
//         sleep(1);
//     }

//     pid_t ret = waitpid(id, nullptr, 0);
//     assert(ret < 0);
//     (void)ret;

//     close(pipefd[1]);
//     return 0;
// }

//child->write, father->read                                                                                                                                                                                                                                                                                                                                                                                                                                                        
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
// 	//父进程从管道读取数据
// 	char buff[64];
// 	while (1){
// 		ssize_t s = read(fd[0], buff, sizeof(buff));
// 		if (s > 0){
// 			buff[s] = '\0';
// 			printf("child send to father:%s\n", buff);
// 		}
// 		else if (s == 0){
// 			printf("read file end\n");
// 			break;
// 		}
// 		else{
// 			printf("read error\n");
// 			break;
// 		}
// 	}
// 	close(fd[0]); //父进程读取完毕，关闭文件
// 	waitpid(id, NULL, 0);
// 	return 0;
// }
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main()
{
	int fd[2] = { 0 };
	if (pipe(fd) < 0){ //使用pipe创建匿名管道
		perror("pipe");
		return 1;
	}
	pid_t id = fork(); //使用fork创建子进程
	if (id == 0){
		//child 
		close(fd[0]); //子进程关闭读端
		char c = 'a';
		int count = 0;
		//子进程一直进行写入，一次写入一个字节
		while (1){
			write(fd[1], &c, 1);
			count++;
			printf("%d\n", count); //打印当前写入的字节数
		}
		close(fd[1]);
		exit(0);
	}
	//father
	close(fd[1]); //父进程关闭写端

	//父进程不进行读取

	waitpid(id, NULL, 0);
	close(fd[0]);
	return 0;
}
