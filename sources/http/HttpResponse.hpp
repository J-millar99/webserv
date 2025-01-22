#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"

enum HttpStatusCode {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
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

class HttpResponse {
    private:
        std::string version;
        int status_code;
        std::string status_message;

        std::string response_line;
        HttpHeader headers;
        std::string response_body;
        
        std::string response_message;

    public:
        HttpResponse();
        HttpResponse(const HttpResponse &ref);
        HttpResponse &operator=(const HttpResponse &ref);
        ~HttpResponse();

        std::string setStatusMessage(int code) const;
        void createErrorPage(int status_code);
        void createPage();
        int length() const;
        std::string message() const;
};

#endif
