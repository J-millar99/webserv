#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpHeader.hpp"

class HttpRequest
{
    private:
        HttpRequest(const HttpRequest &src);
        HttpRequest &operator=(const HttpRequest &src);

        HttpHeader headers;
        std::string method;
        std::string uri;
        std::string version;
        std::string body;

    public:
        HttpRequest();
        ~HttpRequest();

        bool parseHttpRequest(const std::string &raw_request);

        const HttpHeader &getHeaders() const;
        const std::string &getMethod() const;
        const std::string &getUri() const;
        const std::string &getVersion() const;
        const std::string &getBody() const;
};

#endif
