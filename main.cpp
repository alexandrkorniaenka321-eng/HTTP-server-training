#include <iostream>
#include <cstring>
#include <string>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

ssize_t send_all(char *buffer, size_t buffer_size)
{
    return 1;
}

ssize_t recv_all(char *buffer, size_t buffer_size)
{
    return 1;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cout << strerror(errno) << std::endl;
        return -1;
    }

    sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(8080);

    int pton = inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr);
    if (pton == 0)
    {
        std::cout << "Invalid IP\n";
        return 1;
    }
    else if (pton < 0)
    {
        std::cout << "OS [inet_pton()] error: " << strerror(errno) << std::endl;
        return 1;
    }

    if (bind(server_fd, reinterpret_cast<sockaddr *>(&socket_address), sizeof(socket_address))) 
    {
        std::cout << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN)) 
    {
        std::cout << "Listening failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    int user_fd = accept(server_fd, nullptr, nullptr);
    if (user_fd < 0)
    {
        std::cout << "Accepting failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t receive = recv(user_fd, buffer, sizeof(buffer) - 1, 0);

    if (receive > 0)
    {
        std::cout << "Receive data succesfuly\n";
        buffer[receive] = '\0';
        std::cout << buffer << std::endl;
    }
    else if (receive == 0)
    {
        std::cout << "User close connection\n";
    }
    else if (receive == -1)
    {
        std::cout << "Receiving error: " << strerror(errno) << std::endl;
        close(server_fd);
        close(user_fd);
        return 1;
    }

    std::string responce = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 36\r\n\r\n<h1>Hello from C++ HTTP Server!</h1>";

    ssize_t bytes_sended = send(user_fd, responce.c_str(), responce.size(), 0);
    if (bytes_sended < 0)
    {
        std::cout << "Sending error: " << strerror(errno) << std::endl;
    }

    close(user_fd);

    close(server_fd);

    return 0;
}