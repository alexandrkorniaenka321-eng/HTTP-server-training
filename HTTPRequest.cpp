#include "HTTPRequest.h"
#include <cctype>
#include <stdexcept>

HTTPRequest::HTTPRequest(std::string method, std::string request_URL, std::string HTTP_version,
                         std::unordered_map<std::string, std::string> headers, std::string body)
    : method(std::move(method)), request_URL(std::move(request_URL)), HTTP_version(std::move(HTTP_version)),
      headers(std::move(headers)), body(std::move(body)) {}

std::string HTTPRequest::get_url_request() const
{
    return request_URL;
}

void do_lower(std::string &str)
{
    for (char &c : str)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

HTTPRequest parse_http_request(const std::string &http_request)
{
    size_t end_of_request_line = http_request.find("\r\n");
    if (end_of_request_line == std::string::npos)
    {
        throw std::exception();
    }

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
            case 1:
                method = buffer;
                break;
            case 2:
                request_URL = buffer;
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
    if (end_of_headers == std::string::npos)
    {
        throw std::exception();
    }

    std::string key;
    std::string value;
    std::unordered_map<std::string, std::string> headers;

    bool key_value = false;
    while (i < end_of_headers)
    {
        if ((i > 0 && http_request[i] == ' ' && http_request[i - 1] == ':') || http_request[i] == '\n')
        {
        }
        else if (http_request[i] == ':' && !key_value)
        {
            key_value = true;
        }
        else if (http_request[i] == '\r')
        {
            do_lower(key);
            headers.insert(std::make_pair(std::move(key), std::move(value)));

            key.clear();
            value.clear();

            key_value = false;
        }
        else
        {
            if (!key_value)
            {
                key += http_request[i];
            }
            else
            {
                value += http_request[i];
            }
        }
        ++i;
    }

    size_t body_start = end_of_headers + 4;
    size_t body_length = 0;
    auto it = headers.find("content-length");
    std::string body;

    if (it != headers.end())
    {
        body_length = std::stoul(it->second);
    }

    if (body_length > 0)
    {
        for (size_t i = body_start, j = 0; j < body_length; ++j, ++i)
        {
            body += http_request[i];
        }
    }

    return HTTPRequest(std::move(method), std::move(request_URL), std::move(HTTP_version),
                       std::move(headers), std::move(body));
}