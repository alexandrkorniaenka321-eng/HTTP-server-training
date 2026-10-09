#pragma once
#include <string>
#include <unordered_map>

class HTTPResponse
{
public:
    HTTPResponse(std::string HTTP_version, std::string status_code, std::string status_message,
        std::unordered_map<std::string,std::string> headers, std::string body);

    std::string make_http_response() const;

private:
    std::string HTTP_version;
    std::string status_code;
    std::string status_message;
    std::unordered_map<std::string,std::string> headers;
    std::string body;

    std::string make_status_line() const;
    std::string make_headers_line() const;
    std::string make_body() const;


};