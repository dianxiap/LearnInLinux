#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
  pid_t id = fork();
	if (id == 0){
		//child
		int count = 3;
		while (count--){
			printf("child do something...PID:%d, PPID:%d\n", getpid(), getppid());
			sleep(3);
		}
		exit(0);
	}
	//father
	while (1){
		int status = 0;
		pid_t ret = waitpid(id, &status, WNOHANG);
		if (ret > 0){
			printf("wait child success...\n");
			printf("exit code:%d\n", WEXITSTATUS(status));
			break;
		}
		else if (ret == 0){
			printf("father do other things...\n");
			sleep(1);
		}
		else{
			printf("waitpid error...\n");
			break;
   }
  }
  //pid_t ids[10];
  //for(int i=0;i<10;i++)
  //{
  //  pid_t id=fork();
  //  if(id==0)
  //  {
  //    printf("child process created successfully...PID:%d\n",getpid());
  //    sleep(3);
  //    exit(i);  //将子进程的推出码设置为该子进程PID在数组ids中的下标
  //  }
  //  ids[i]=id;
  //}
  //for(int i=0;i<10;i++)
  //{
  //  int status=0;
  //  pid_t ret=waitpid(ids[i],&status,0);
  //  if(ret>=0)
  //  {
  //    //wait pid success
  //    printf("wait child success...PID:%d\n",ids[i]);
  //    if(WIFEXITED(status))
  //    {
  //      //exit nomal
  //      printf("exit code:%d\n",WEXITSTATUS(status));
  //    }
  //    else 
  //    {
  //      //signal killed
  //      printf("killed by signal %d\n",status & 0x7f);
  //    }
  //  }
  //}
  //pid_t id = fork();
	//if (id == 0){
	//	//child          
	//	int count = 10;
	//	while (count--){
	//		printf("I am child...PID:%d, PPID:%d\n", getpid(), getppid());
	//		sleep(1);
	//	}
	//	exit(0);
	//}
	////father           
	//int status = 0;
	//pid_t ret = waitpid(id, &status, 0);
	//if (ret >= 0){
	//	//wait success                    
	//	printf("wait child success...\n");
	//	if (WIFEXITED(status)){
	//		//exit normal                                 
	//		printf("exit code:%d\n", WEXITSTATUS(status));
	//	}
	//	else{
	//		//signal killed                              
	//		printf("killed by siganl %d\n", status & 0x7F);
	//	}
	//}
	//sleep(3);
	//return 0;
	//pid_t id = fork();
	//if(id == 0){
	//	//child
	//	int count = 10;
	//	while(count--){
	//		printf("I am child...PID:%d, PPID:%d\n", getpid(), getppid());
	//		sleep(1);
	//	}
	//	exit(0);
	//}
	////father
	//int status = 0;
  //pid_t ret = wait(&status);
	//if(ret > 0){
	//	//wait success
	//	printf("wait child success...\n");
	//	if(WIFEXITED(status)){
	//		//exit normal
	//		printf("exit code:%d\n", WEXITSTATUS(status));
	//	}
	//}
	//sleep(3);
  //return 0;
  //show();
  //vareturn 0;
 // for(int i=0;i<150;i++)
 // {
 //   printf("%d:%s\n",i,strerror(i));
 // }
 // return 0;
  //binprintf("hello world\n");
  //vareturn 0;
  //printf("before ：PID is %d\n",getpid());
  //pid_t id=fork();
  //if(id==-1)
  //{
  //  perror("fork error");
  //  exit(1);
  //}
  //printf("after: PID is %d, return is %d\n",getpid(),id);
  //return 0;
}
