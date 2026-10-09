#include "HTTPRequest.h"
#include <cctype>

HTTPRequest::HTTPRequest(std::string method,std::string request_URL,std::string HTTP_version,
    std::unordered_map<std::string,std::string> headers,std::string body)
    : method(std::move(method)),request_URL(std::move(request_URL)),HTTP_version(std::move(HTTP_version)),
    headers(std::move(headers)),body(std::move(body)) {}       

std::string HTTPRequest::make_request_line() const
{
    return method + ' ' + request_URL + ' ' + HTTP_version + "\r\n";
}

std::string HTTPRequest::make_headers_block() const
{
    std::string headers_block;

    for (const auto &i : headers)
    {
        headers_block += i.first + ':' + i.second + "\r\n";
    }
    return headers_block + "\r\n";
}

void do_lower(std::string& str)
{
    for(char &c : str)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

HTTPRequest parse_http_request(const std::string &http_request)
{
    size_t end_of_request_line = http_request.find("\r\n");

    std::string method;
    std::string request_URL;
    std::string HTTP_version;
    int type = 0;

    int i = 0;
    std::string buffer;
    while (i < end_of_request_line)
    {
        if (http_request[i] == ' ')
        {
            ++type;

            switch (type)
            {
            case 1: method = buffer;
                break;
            case 2: request_URL = buffer;
                break;
            }
            buffer.clear();
        }
        else
        {
            buffer += http_request[i];
        }
        ++i;
    }
    HTTP_version = buffer;
    i += 2;

    size_t end_of_headers = http_request.find("\r\n\r\n");

    std::string key;
    std::string value;
    std::unordered_map<std::string,std::string> headers;

    bool key_value = false;
    while(i < end_of_headers)
    {
        if((i > 0 && http_request[i] == ' ' && http_request[i - 1] == ':') || http_request[i] == '\n')
        {
            ++i;
            continue;
        }
        else if(http_request[i] == ':')
        {
            key_value = true;
            continue;
        }
        else if(http_request[i] == '\r')
        {
            do_lower(key);
            headers.insert(std::make_pair(std::move(key),std::move(value)));

            key.clear();
            value.clear();

            key_value = false;
            ++i;

            continue;
        }

        if(!key_value)
        {
            key += http_request[i];
        }
        else 
        {
            value += http_request[i];
        }
        ++i;
    }


    size_t body_start = end_of_headers + 4;
    size_t body_length = 0;
    auto it = headers.find("content-length");
    std::string body;
    
    if(it != headers.end())
    {
        body_length = std::stoul(it->second);
    }

    if(body_length > 0)
    {
        for(size_t i = body_start,j = 0; j < body_length; ++j,++i)
        {
            body += http_request[i];
        }
    }

    return HTTPRequest(std::move(method),std::move(request_URL),std::move(HTTP_version),
        std::move(headers),std::move(body));
}