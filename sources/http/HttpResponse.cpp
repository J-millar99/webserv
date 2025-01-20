#include "HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpResponse &ref) {
    version = ref.version;
    status_code = ref.status_code;
    status_message = ref.status_message;
    body = ref.body;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &ref) {
    if (this != &ref) {
        version = ref.version;
        status_code = ref.status_code;
        status_message = ref.status_message;
        body = ref.body;
    }
    return *this;
}

HttpResponse::~HttpResponse() { }
HttpResponse::HttpResponse() { }

void HttpResponse::setVersion(const std::string &version) {
    this->version = version;
}

void HttpResponse::setStatusCode(int status_code) {
    this->status_code = status_code;
}

void HttpResponse::setStatusMessage(const std::string &status_message) {
    this->status_message = status_message;
}

void HttpResponse::setBody(const std::string &body) {
    this->body = body;
}
