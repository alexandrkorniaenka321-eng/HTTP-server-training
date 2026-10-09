#include "HTTPResponce.h"
#include <algorithm>
#include <cctype>

HTTPResponse::HTTPResponse(std::string HTTP_version, int status_code, std::string status_message,
        std::unordered_map<std::string,std::string> headers, std::string body) 
        : HTTP_version(std::move(HTTP_version)), status_code(status_code),
        status_message(std::move(status_message)), headers(std::move(headers)),body(std::move(body)) {}

std::string HTTPResponse::make_http_response() const
{
    return make_status_line() + make_headers_line() + make_body();
}

void HTTPResponse::set_http_version(std::string HTTP_version)
{
    this->HTTP_version = std::move(HTTP_version);
}

void HTTPResponse::set_status_code(int status_code)
{
    this->status_code = status_code;
}

void HTTPResponse::set_status_message(std::string status_message)
{
    this->status_message = std::move(status_message);
}

void HTTPResponse::set_header(std::string key,std::string value)
{
    std::transform(key.begin(),key.end(),key.begin(),[](unsigned char c){
        return std::tolower(c);
    });
    if(key == "content-length") return;

    headers[std::move(key)] = std::move(value);
}

void HTTPResponse::set_body(std::string body)
{
    this->body = std::move(body);
}

std::string HTTPResponse::make_status_line() const
{
    return HTTP_version + ' ' + std::to_string(status_code) + ' ' + status_message + "\r\n";
}

std::string HTTPResponse::make_headers_line() const
{
    std::string headers_line;
    size_t content_length = body.size();

    headers_line += "Content-Length: " + std::to_string(content_length) + "\r\n";
    
    for(const auto& it : headers)
    {
        headers_line += it.first + ": " + it.second + "\r\n";
    }
    return  headers_line + "\r\n";
}

std::string HTTPResponse::make_body() const
{
    return body;
}