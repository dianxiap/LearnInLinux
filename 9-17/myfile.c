#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    //C语言提供
    printf("hello printf\n");
    fprintf(stdout,"hello fprintf\n");
    const char *s="hello fputs\n";
    fputs(s,stdout);

    //os提供
    const char *ss="hello write\n";
    write(1,ss,strlen(ss));

    //fork();
    
}
