#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <utils.hpp>
#include "HttpHeader.hpp"

class HttpResponse
{
    private:
        HttpResponse(const HttpResponse &src);
        HttpResponse &operator=(const HttpResponse &src);

        HttpHeader headers;
        std::string version;
        int status_code;
        std::string status_message;
        std::string body;
        
    public:
        HttpResponse();
        ~HttpResponse();

        void setResponse(const std::string &version, int status_code, const std::string &status_message, const std::string &body);
        
};

#endif
