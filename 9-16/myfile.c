#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
  if(argc!=2)
  {
    return 2;
  }
  int fd=open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
  //int fd=open("log.txt",O_WRONLY|O_CREAT,0666);
  if(fd<0)
  {
    perror("open");
    return 1;
  }
  dup2(fd,1);
  fprintf(stdout,"%s\n",argv[1]);
  close(fd);
  return 0;
  ////////////////////////////////////////////////////////
  //close(0);
  //int fd=open("log.txt",O_RDONLY);
  //if(fd<0)
  //{
  //  perror("open");
  //  return 1;
  //}
  //printf("fd: %d\n",fd);

  //char buffer[64];
  //fgets(buffer,sizeof buffer,stdin);

  //printf("%s\n", buffer);
  ////////////////// /////////////////////////////////////
  // //close(0);
  // close(1);
  // //fd的分配规则：最小的，没有被占用的文件描述符
  // //O_TRUNC 打开文件时清空文件原有内容
  // int fd=open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
  // if(fd<0)
  // {
  //    perror("open");
  //    return 1;
  // }

  // //本来都是打印到显示器上的
  // //但是都写入到log.txt中了
  // //输出重定向？
  // printf("fd: %d\n",fd);
  // printf("fd: %d\n",fd);
  // printf("fd: %d\n",fd);
  // printf("fd: %d\n",fd);
  // printf("fd: %d\n",fd);
  // fprintf(stdout,"hello fprentf\n");
  // const char *s="hello fwrite\n";
  // fwrite(s,strlen(s),1,stdout);

  // fflush(stdout);
  // //stdout->_fileno=1
  // close(fd);
  // return 0;

   ///////////////////////////////////////////////////
   //printf("stdin: %d\n",stdin->_fileno);
   //printf("stdout: %d\n",stdout->_fileno);
   //printf("stderr: %d\n",stderr->_fileno);

   // //设置当前进程掩码
   // umask(0);
   // int fd=open("log.txt",O_WRONLY|O_CREAT,0666); //rw-rw-rw-
   // if(fd<0)
   // {
   //     perror("open");
   //     return 1;
   // }
   // const char *s="hello write\n";
   // write(fd,s,strlen(s));

   // printf("open success, fd: %d\n", fd);
   // 
   // close(fd);
   // return 0;
}
