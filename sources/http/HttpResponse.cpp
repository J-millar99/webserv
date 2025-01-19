#include "HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpResponse &ref) { (void)ref; } /* Unused */
HttpResponse &HttpResponse::operator=(const HttpResponse &ref) { (void)ref; return *this; } /* Unused */
HttpResponse::~HttpResponse() { }

HttpResponse::HttpResponse() {
    
}

void HttpResponse::setResponse(const std::string &version, int status_code, const std::string &status_message, const std::string &body) {
    this->version = version;
    this->status_code = status_code;
    this->status_message = status_message;
    this->body = body;
}