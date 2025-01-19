#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const HttpRequest &ref) { (void)ref; } /* Unused */
HttpRequest &HttpRequest::operator=(const HttpRequest &ref) { (void)ref; return *this; } /* Unused */
HttpRequest::~HttpRequest() { }

HttpRequest::HttpRequest() {
    
}

bool HttpRequest::parseHttpRequest(const std::string &raw_request)
{
    std::vector<std::string> lines = split(raw_request, "\r\n");
    std::vector<std::string> first_line = split(lines[0], " ");

    if (first_line.size() != 3)
        return false;

    method = first_line[0];
    uri = first_line[1];
    version = first_line[2];

    for (size_t i = 1; i < lines.size(); i++)
    {
        std::vector<std::string> header = split(lines[i], ": ");
        if (header.size() == 2)
            headers.addHeader(header[0], header[1]);
    }

    body = lines[lines.size() - 1];

    return true;
}

const HttpHeader &HttpRequest::getHeaders() const
{
    return headers;
}

const std::string &HttpRequest::getMethod() const
{
    return method;
}

const std::string &HttpRequest::getUri() const
{
    return uri;
}

const std::string &HttpRequest::getVersion() const
{
    return version;
}

const std::string &HttpRequest::getBody() const
{
    return body;
}