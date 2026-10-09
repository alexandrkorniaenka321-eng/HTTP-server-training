#include <iostream>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "HTTPRequest.h"
#include "HTTPResponce.h"

ssize_t send_all(int socket_fd, const char *buffer, size_t buffer_size, int flags)
{
    size_t total_sended = 0;
    while (total_sended < buffer_size)
    {
        ssize_t send_result = send(socket_fd, buffer + total_sended, buffer_size - total_sended, flags);
        if (send_result == 0)
        {
            return 0;
        }
        else if (send_result == -1)
        {
            return -1;
        }
        total_sended += send_result;
    }
    return total_sended;
}

std::string parse_http(const char *buffer, size_t buffer_size)
{
    std::string client_text = buffer;

    int pos = client_text.find("HTTP");
    client_text.clear();

    if (pos != std::string::npos)
    {
        while (pos >= 0)
        {
            pos--;
            if (buffer[pos] == '/')
            {
                std::reverse(client_text.begin(), client_text.end());
                return client_text;
            }
            else if (buffer[pos] == ' ')
            {
                continue;
            }
            client_text += buffer[pos];
        }
    }
    return client_text;
}

std::string make_http_answer(const std::string &client_text)
{
    std::string body = "<h1>" + client_text + "</h1>";
    size_t contex_lenght = body.size();
    return "HTTP/1.1 200 OK\nContext-Type: text/html/\nContext-Length: " + std::to_string(contex_lenght) + "\n\n" + body;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cout << "Socket inicialization error: " << strerror(errno) << std::endl;
        return -1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cout << "Socket options error: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
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
        close(server_fd);
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

    while (true)
    {
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
            std::cout << "Receive data succesfuly\nbytes received: " << receive << '\n';
            buffer[receive] = '\0';
            std::cout << buffer << std::endl;
        }
        else if (receive == 0)
        {
            std::cout << "User close connection\n";
            close(server_fd);
            close(user_fd);
            return 1;
        }
        else if (receive == -1)
        {
            std::cout << "Receiving error: " << strerror(errno) << std::endl;
            close(server_fd);
            close(user_fd);
            return 1;
        }

        HTTPRequest http_request = parse_http_request(std::string(buffer));
        HTTPResponse http_response;

        if (http_request.get_url_request() == "/index.html")
        {
            http_response.set_body(R"(<!DOCTYPE html>
            <html lang="ru">
            <head>
            <meta charset="UTF-8">
            <title>Мой C++ Сервер</title>
            </head>
            <body>
            <h1>Сервер работает!</h1>
            <p>Это ответ, сгенерированный C++ сервером на чистых сокетах.</p>
            <ul>
            <li><a href="/json">Посмотреть JSON API</a></li>
            <li><a href="/not-found">Проверить 404 страницу</a></li>
            </ul>
            </body>
            </html>)");
        }
        else if (http_request.get_url_request() == "/api/status" || http_request.get_url_request() == "/json")
        {
            http_response.set_body(R"({
            "status": "success",
            "server": "Custom C++ HTTP Server",
            "version": "1.0.0",
            "uptime_seconds": 3600,
            "features": ["TCP Sockets", "HTTP Parsing", "Routing"]
            })");
        }
        else
        {
            http_response.set_body(R"(<!DOCTYPE html>
            <html lang="ru">
            <head>
            <meta charset="UTF-8">
            <title>404 - Страница не найдена</title>
            <style>
            body { font-family: sans-serif; text-align: center; padding-top: 50px; }
            h1 { color: #e74c3c; font-size: 48px; }
            </style>
            </head>
            <body>
            <h1>404</h1>
            <h2>Упс! Запрошенная страница не найдена.</h2>
            <p><a href="/index.html">Вернуться на главную</a></p>
            </body>
            </html>)");
        }

        std::string response = http_response.make_http_response();

        std::cout << "========== RESPONSE ==========\n";
        std::cout << response;
        std::cout << "\n==============================\n";

        ssize_t bytes_sended =
        send_all(user_fd, response.c_str(), response.size(), 0);
        if (bytes_sended > 0)
        {
            std::cout << "Bytes sended: " << bytes_sended << std::endl;
        }
        else if (bytes_sended < 0)
        {
            std::cout << "Sending error: " << strerror(errno) << std::endl;
        }

        close(user_fd);
    }

    close(server_fd);

    return 0;
}