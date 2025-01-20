#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpHeader.hpp"

class HttpRequest
{
    private:
        std::string raw_request;

        HttpHeader headers;
        std::string method;
        std::string uri;
        std::string version;
        std::string body;

    public:
        HttpRequest();
        HttpRequest(const HttpRequest &ref);
        HttpRequest &operator=(const HttpRequest &ref);
        ~HttpRequest();

        bool parseHttpRequest();

        void addRawRequest(const std::string &buffer);
        const HttpHeader &getHeaders() const;
        const std::string &getMethod() const;
        const std::string &getUri() const;
        const std::string &getVersion() const;
        const std::string &getBody() const;
        void printInfo() const;
};

#endif
