#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"

using namespace std;

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        cout<<"Usage\n\t"<<argv[0]<<" serverip serverport"<<endl;
        exit(2);
    }
    uint16_t serverport=atoi(argv[2]);
    string serverip=argv[1];

    int sock=socket(AF_INET,SOCK_DGRAM,0);
    assert(sock>0);
    
    struct sockaddr_in server;
    bzero(&server,sizeof server);
    server.sin_family=AF_INET;
    server.sin_port=htons(serverport);
    server.sin_addr.s_addr=inet_addr(serverip.c_str());

    string buffer;
    while(1)
    {
        cout<<"Please Enter# ";
        getline(cin,buffer);
        sendto(sock,buffer.c_str(),buffer.size(),0,(struct sockaddr*)&server,sizeof server);
    }
    
    return 0;
}