#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define NUM 1024
#define SIZE 32
#define SEP " "

char cmd_line[NUM];
char *g_argv[SIZE];
char g_myval[64];

#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
#define NONE_REDIR 0

int redir_status=NONE_REDIR;

char *CheckRedir(char* start)
{
  assert(start);
  char* end=start+strlen(start)-1;
  while(end>=start)
  {
    if(*end=='>')
    {
      if(*(end-1)=='>')
      {
        redir_status=APPEND_REDIR;
        *(end-1)='\0';
        end++;
        break;
      }
      redir_status=OUTPUT_REDIR;
      *end='\0';
      end++;
      break;
    }
    else if(*end=='<')
    {
      redir_status=INPUT_REDIR;
      *end='\0';
      end++;
      break;
    }
    else 
    {
      end--;
    }
  }
  if(end>=start)
  {
    return end;
  }
  else return NULL;
}

int main()
{
  extern char** environ;
  while(1)
  {
    printf("[root@localhost myshell]# ");
    fflush(stdin);
    memset(cmd_line,'\0',sizeof cmd_line);
   
    if(fgets(cmd_line,sizeof cmd_line,stdin)==NULL)
    {
      continue;
    }
    cmd_line[strlen(cmd_line)-1]='\0';
    
    char *sep=CheckRedir(cmd_line);

    g_argv[0]=strtok(cmd_line,SEP);
    int index=1;
   
    if(strcmp(g_argv[0],"ls")==0)
    {
      g_argv[index++]="--color=auto";
    }
    if(strcmp(g_argv[0],"ll")==0)
    {
      g_argv[0]="ls";
      g_argv[index++]="-l";
      g_argv[index++]="--color=auto";
    }

    while(g_argv[index++]=strtok(NULL,SEP));

    if(strcmp(g_argv[0],"export")==0&&g_argv[1]!=NULL)
    {
      //将要设置的环境变量保存一份副本，防止在创建子进程时刷新g_argv里的内容
      strcpy(g_myval,g_argv[1]);
      int ret=putenv(g_myval);
      if(ret==0)printf("%s export succes\n",g_argv[1]);
      continue;
    }

    if(strcmp(g_argv[0],"cd")==0)
    {
  
      if(g_argv[1]!=NULL)chdir(g_argv[1]);
      continue;
    }      

    pid_t id=fork();
    if(id==0)
    {
      if(sep!=NULL)
      {
        int fd=-1;
        switch(redir_status)
        {
          case INPUT_REDIR:
						fd=open(sep,O_RDONLY);
						dup2(fd,0);
						break;						
          case OUTPUT_REDIR:
            fd=open(sep,O_WRONLY|O_TRUNC|O_CREAT,0666);
            dup2(fd,1);
            break;
          case APPEND_REDIR:
            fd=open(sep,O_WRONLY|O_APPEND|O_CREAT,0666);
            dup2(fd,1);
            break;
          default:
            printf("bug?\n");
            break;
        }
      }

     // printf("下面的功能让子进程执行\n");
     // printf("child, MYVAL: %s\n",getenv("MYVAL"));
     // printf("child, PATH: %s\n",getenv("PATH"));
      execvp(g_argv[0],g_argv);
      exit(1);
    }

    int status=0;
    pid_t ret=waitpid(id,&status,0);
    if(ret>0)printf("exit code: %d\n",WEXITSTATUS(status));
  }
}
