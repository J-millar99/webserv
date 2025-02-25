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

static std::string readErrorPage(int status_code) {
    std::stringstream ss;
    int flag = status_code / 100;
    ss << "page/" << flag << "xx/" << status_code << ".html";

    std::ifstream file(ss.str());
    if (!file.is_open())
        // 파일 열기 실패시 기본 에러 메시지 반환
        return "<html><head><title>Error</title></head><body><h1>Error</h1><p>Something went wrong</p></body></html>";

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

static std::string sourcePath(const std::string &extension) {
    std::string::size_type dotPos = extension.find_last_of('.');
    std::string path = extension.substr(dotPos + 1);
    std::string ret;
    if (path == "css")
        ret = "style" + extension;
    else if (path == "jpg" || path == "png" || path == "svg")
        ret = "icon" + extension;
    else
        ret = extension;
    return ret;
}

static std::string readPage(const HttpRequest &request) {
    std::stringstream ss;
    ss << "page/" << sourcePath(request.getUri());

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
    this->headers.addHeader("Content-Type", "text/html; charset=UTF-8");
    this->headers.addHeader("Server", "WebServe");
    this->headers.addHeader("Date", getCurrentHttpDate());
    // create response body
    this->response_body = readErrorPage(status_code);
    this->headers.addHeader("Content-Length", std::to_string(this->response_body.length()));

    // create response message
    this->response_message = this->response_line + "\r\n" + this->headers.getHeaders() + "\r\n" + this->response_body;
}

static std::string contentType(const std::string &uri) {
    std::string::size_type dotPos = uri.find_last_of('.');
    std::string extension = uri.substr(dotPos + 1);
    std::string ret;
    if (extension == "jpg" || extension == "png" || extension == "svg")
        ret = "image/" + extension + "+xml";
    else
        ret = "text/" + extension;
    return ret;
}

void HttpResponse::createPage(const HttpRequest &request) {
    this->version = "HTTP/1.1";
    this->status_code = OK;
    this->status_message = setStatusMessage(OK);

    this->response_line = this->version + " " + std::to_string(this->status_code) + " " + this->status_message;
    std::string type = contentType(request.getUri());
    std::cout << type << std::endl;
    // create response headers
    this->headers.addHeader("Content-Type", type + "; charset=UTF-8");
    this->headers.addHeader("Server", "WebServe");
    this->headers.addHeader("Date", getCurrentHttpDate());

    // create response body
    this->response_body = readPage(request);
    this->headers.addHeader("Content-Length", std::to_string(this->response_body.length()));

    // create response message
    this->response_message = this->response_line + "\r\n" + this->headers.getHeaders() + "\r\n" + this->response_body;
}
