#include "HTTPResponce.h"

HTTPResponse::HTTPResponse(std::string HTTP_version, std::string status_code, std::string status_message,
        std::unordered_map<std::string,std::string> headers, std::string body) 
        : HTTP_version(std::move(HTTP_version)), status_code(std::move(status_code)),
        status_message(std::move(status_message)), headers(std::move(headers)),body(std::move(body)) {}


std::string HTTPResponse::make_status_line() const
{
    return HTTP_version + ' ' + status_code + ' ' + status_message + "\r\n";
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

std::string HTTPResponse::make_http_response() const
{
    return make_status_line() + make_headers_line() + make_body();
}