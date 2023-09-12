#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM 1024
#define SIZE 32
#define SEP " "

//保存完整的命令行字符串
char cmd_line[NUM];
//保存打散之后的命令行字符串
char *g_argv[SIZE];

//shell执行原理：通过让子进程执行命令（进程替换），父进程等待&&解析命令
int main()
{
  //0.命令行解释器，一定是一个常驻内存的进程，不退出
  while(1)
  {
    //1.打印提示信息
    printf("[minshell@localhost myshell]# ");
    fflush(stdin); //将上面的提示信息从缓冲区里刷新出来
    memset(cmd_line,'\0',sizeof cmd_line);
    //2.获取用户的键盘输入[输入的是各种指令和选项："ls -a -l -i"]
    if(fgets(cmd_line,sizeof cmd_line,stdin)==NULL)
    {
      continue;
    }
    cmd_line[strlen(cmd_line)-1]='\0';
    //3.命令行字符串解析："ls -a -l -i" -> "ls" "-a" "-l" "-i"
    g_argv[0]=strtok(cmd_line,SEP); //第一次调用strtok，要传入原始字符串
    int index=1;
    if(strcmp(g_argv[0],"ls")==0)
    {
      g_argv[index++]="--color=auto";
    }
    if(strcmp(g_argv[0],"ll")==0)
    {
      g_argv[0]="ls";
      g_argv[index++]="-1";
      g_argv[index++]="--color=auto";
    }
    while(g_argv[index++]=strtok(NULL,SEP)); //第二次调用strtok，如果还要解析原始字符串，传入NULL
    //4.内置命令，让父进程（shell）自己执行的命令，我们叫做内置命令，内建命令
    //内建命令本质就是shell中的一个函数调用
    if(strcmp(g_argv[0],"cd")==0)
    {
      if(g_argv[1]!=NULL)chdir(g_argv[1]);
      continue;
    }
    //5.fork()
    pid_t id=fork();
    if(id==0)
    {
      printf("下面的功能让子进程执行\n");
      execvp(g_argv[0],g_argv);
      exit(1);
    }
    int status=0;
    pid_t ret=waitpid(id,&status,0);
    if(ret>0)printf("exit code: %d\n",WEXITSTATUS(status));
  }
}
