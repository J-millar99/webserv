#include "Server.hpp"

static int methodBitConverter(const std::string &method) {
    if (method == "GET")
        return GET;
    else if (method == "POST")
        return POST;
    else if (method == "DELETE")
        return DELETE;
    return 0;
}

int Server::isValidRequest(const HttpRequest &request) {
    int status;

    if ((status = isValidRequestLine(request)) != 200 ||
        (status = isValidRequestHeader(request)) != 200 ||
        (status = isValidRequestBody(request)) != 200)
        return status;

    return 200;
}

// 에러 코드는 구체적 명시, 성공 코드는 200으로 설정
int Server::isValidRequestLine(const HttpRequest &request) {
    // 요청 라인 사이즈 검증
    if (request.getUri().size() > MAX_REQUEST_LINE_SIZE)
        return URI_TOO_LONG; // 414
    // 메소드 검증
    std::string request_method = request.getMethod();
    if (request_method != "GET" && request_method != "POST" && request_method != "DELETE")
        return METHOD_NOT_ALLOWED; // 405
    // URI 검증
    std::string request_uri = request.getUri();
        
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        // uri가 일치하면 메소드 검증
        std::cerr << "요청 uri: " <<request_uri << " ";
        std::cerr << "현재 uri: " << it->url << std::endl;
        if (request_uri == it->url) {
            if ((it->methods & methodBitConverter(request_method)) != 0)
                return OK; // 200
            else
                return METHOD_NOT_ALLOWED; // 405
        }
    }
    return NOT_FOUND; // 404
}

int Server::isValidRequestHeader(const HttpRequest &request) {
    std::string host = request.getHeaders().getHeader("Host");
    // Host 헤더가 없으면 400 반환
    if (host == "")
        return BAD_REQUEST; // 400
    size_t pos = host.find(':');
    if (pos != std::string::npos)
        host = host.substr(0, pos);
    if (std::find(server_names.begin(), server_names.end(), host) == server_names.end())
        return NOT_FOUND; // 404
    return OK; // 200
}

// 바디 사이즈가 Content-Length보다 크면 413 반환
int Server::isValidRequestBody(const HttpRequest &request) {
    // limit_client_body_size가 0이면 바디 사이즈 제한 없음
    if (request.getBody().size() == 0)
        return OK; // 200
    // Content-Length 헤더가 있는 경우
    HttpHeader headers = request.getHeaders();
    if (headers.getHeader("Content-Length") != "")
        if (request.getBody().size() > static_cast<size_t>(limit_client_body_size))
            return PAYLOAD_TOO_LARGE; // 413
    return OK; // 200
}