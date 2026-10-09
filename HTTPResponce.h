#pragma once
#include <string>
#include <unordered_map>

class HTTPResponse
{
public:
    HTTPResponse(std::string HTTP_version = "HTTP/1.1",
                int status_code = 200,
                std::string status_message = "OK",
                std::unordered_map<std::string,std::string> headers = {},
                std::string body = "<h1>Hello</h1>");

    std::string make_http_response() const;

    void set_http_version(std::string HTTP_version);
    void set_status_code(int status_code);
    void set_status_message(std::string status_message);
    void set_header(std::string key,std::string value);
    void set_body(std::string body);

private:
    std::string HTTP_version;
    int status_code;
    std::string status_message;
    std::unordered_map<std::string,std::string> headers;
    std::string body;

    std::string make_status_line() const;
    std::string make_headers_line() const;
    std::string make_body() const;

};