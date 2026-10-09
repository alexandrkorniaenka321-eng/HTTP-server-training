#pragma once

#include <string>
#include <unordered_map>

class HTTPRequest
{
public:
    HTTPRequest(std::string method,std::string request_URL,std::string HTTP_version,
        std::unordered_map<std::string,std::string> headers,std::string body);

    std::string get_url_request() const;

private:
    std::string method;
    std::string request_URL;
    std::string HTTP_version;
    std::unordered_map<std::string,std::string> headers;
    std::string body;

};

void do_lower(std::string& str);
HTTPRequest parse_http_request(const std::string& http_request);
