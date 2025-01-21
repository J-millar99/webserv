#include "HttpResponse.hpp"

HttpResponse::~HttpResponse() { }
HttpResponse::HttpResponse() { }

static std::string getCurrentHttpDate() {
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    char buffer[128];
    const char* format = "%a, %d %b %Y %H:%M:%S GMT";
    
    strftime(buffer, sizeof(buffer), format, gmt);
    return std::string(buffer);
}

static std::string readPage(int status_code) {
    std::stringstream ss;
    ss << "sources/" << "page/" << status_code << ".html";

    std::ifstream file(ss.str());
    if (!file.is_open())
        // 파일 열기 실패시 기본 에러 메시지 반환
        return "<html><head><title>Error</title></head><body><h1>Error</h1><p>Something went wrong</p></body></html>";

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

HttpResponse::HttpResponse(const HttpResponse &ref) {
    headers = ref.headers;
    version = ref.version;
    status_code = ref.status_code;
    status_message = ref.status_message;
    response_body = ref.response_body;
    response_line = ref.response_line;
    response_message = ref.response_message;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &ref) {
    if (this != &ref) {
        headers = ref.headers;
        version = ref.version;
        status_code = ref.status_code;
        status_message = ref.status_message;
        response_body = ref.response_body;
        response_line = ref.response_line;
        response_message = ref.response_message;
    }
    return *this;
}

int HttpResponse::length() const {
    return response_message.length();
}

std::string HttpResponse::message() const {
    return response_message;
}

std::string HttpResponse::setStatusMessage(int code) const {
    switch (code) {
        case OK: return "OK";
        case BAD_REQUEST: return "BAD_REQUEST";
        case NOT_FOUND: return "NOT_FOUND";
        case METHOD_NOT_ALLOWED: return "METHOD_NOT_ALLOWED";
        case REQUEST_TIMEOUT: return "REQUEST_TIMEOUT";
        case PAYLOAD_TOO_LARGE: return "PAYLOAD_TOO_LARGE";
        case URI_TOO_LONG: return "URI_TOO_LONG";
        case INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
        case NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case BAD_GATEWAY: return "BAD_GATEWAY";
        case SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
        case GATEWAY_TIMEOUT: return "GATEWAY_TIMEOUT";
        default: return "UNKNOWN_STATUS_CODE";
    }
}

void HttpResponse::createErrorPage(int status_code) {
    this->version = "HTTP/1.1";
    this->status_code = status_code;
    this->status_message = setStatusMessage(status_code);

    // create response line
    this->response_line = this->version + " " + std::to_string(this->status_code) + " " + this->status_message;
    // create response headers
    this->headers.addHeader("Content-Type", "text/html");
    this->headers.addHeader("Connection", "close");
    this->headers.addHeader("Server", "WebServe");
    this->headers.addHeader("Date", getCurrentHttpDate());
    // create response body
    this->response_body = readPage(status_code);
    this->headers.addHeader("Content-Length", std::to_string(this->response_body.length()));

    // create response message
    this->response_message = this->response_line + "\r\n" + this->headers.getHeaders() + "\r\n" + this->response_body;
}

void HttpResponse::createPage() {
    this->version = "HTTP/1.1";
    this->status_code = OK;
    this->status_message = setStatusMessage(OK);
    this->response_body = "<html><head><title>Page</title></head><body><h1>Page</h1><p>Page content</p></body></html>";
}
