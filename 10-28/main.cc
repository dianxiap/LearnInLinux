#include "Sock.hpp"

int main()
{
    Sock sock;
    int listensock= sock.Socket();
    sock.Bind(listensock,8080);

    sock.Listen(listensock);

    while(true)
    {
        std::string clientip;
        uint16_t clientport;
        int sockfd=sock.Accept(listensock,&clientip,&clientport);
        if(sockfd>0)
        {
           std::cout<<"["<<clientip<<":"<<clientport<<"]# "<<sockfd;
        }

        sleep(10);

        close(sockfd);

        std::cout<<sockfd<<" closed"<<std::endl;
    }
}
