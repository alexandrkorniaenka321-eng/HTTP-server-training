#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int socket_fd = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in socket_address;
    memset(&socket_address,0,sizeof(socket_address));

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(8080);
   
    if(!inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr)) //127.0.0.1 localhost now
    {
        std::cout << "Invalid IP\n";
    }

    if(bind(socket_fd,reinterpret_cast<sockaddr*>(&socket_address),sizeof(socket_address))) //Add binding error handling.
    {
        std::cout << "socket binding error\n";
    }

    if(listen(socket_fd,SOMAXCONN)) //maximum connections which can be suppoted by OS(Linux)
    {
        std::cout << "Socket listening error\n";
    } 
    

    return 0;
}