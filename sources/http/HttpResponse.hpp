#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"

enum HttpStatusCode
{
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_TIMEOUT = 408,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504
};

class HttpResponse
{
    private:

        HttpHeader headers;
        std::string version;
        int status_code;
        std::string status_message;
        std::string body;
        
    public:
        HttpResponse();
        HttpResponse(const HttpResponse &ref);
        HttpResponse &operator=(const HttpResponse &ref);
        ~HttpResponse();

        void setVersion(const std::string &version);
        void setStatusCode(int status_code);
        void setStatusMessage(const std::string &status_message);
        void setBody(const std::string &body);
};

#endif
