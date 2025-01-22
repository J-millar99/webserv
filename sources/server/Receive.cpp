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

HttpRequest Server::recvHttpRequest(int client_socket) {
    HttpRequest request;
    char buffer[BUFFER_SIZE];

    std::string raw_request;
    while (true) {
        ssize_t n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            close(client_socket);
            // 문제가 생겼을 때 500번대 페이지 만들어 전송
            return request;
        }
        
        raw_request.append(buffer, n);
        
        // 헤더의 끝(\r\n\r\n)을 찾습니다
        if (raw_request.find("\r\n\r\n") != std::string::npos)
            break;
    }

    request.addRawRequest(raw_request);
    
    std::string temp = request.getHeaders().getHeader("Content-Length");
    int content_length = stringToInt(temp);
    if (content_length > 0) {
        std::vector<char> body_buffer(content_length);
        size_t remaining = content_length;
        
        while (remaining > 0) {
            ssize_t n = recv(client_socket, 
                           body_buffer.data() + (content_length - remaining),
                           remaining, 0);
            if (n <= 0) {
                close(client_socket);
                return request;
            }
            remaining -= n;
        }
        request.addRawRequest(std::string(body_buffer.begin(), body_buffer.end()));
    }
    
    if (!request.parseHttpRequest())
        close(client_socket);
    return request;
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
    // if (isDirectory(request_uri))
    //     request_uri += "/"; // 디렉토리일 경우 / 추가
    // if (isRegularFile(request_uri))
        
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        // uri가 일치하면 메소드 검증
        std::cerr << "요청 uri: " <<request_uri << " ";
        std::cerr << "현재 uri: " << it->url_type << std::endl;
        if (request_uri == it->url_type) {
            if ((it->methods & methodBitConverter(request_method)) != 0)
                return OK; // 200
            else
                return METHOD_NOT_ALLOWED; // 405
        }
    }
    return NOT_FOUND; // 404
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

int Server::isValidRequestHeader(const HttpRequest &request) {
    // Host 헤더가 없으면 400 반환
    if (request.getHeaders().getHeader("Host") == "")
        return BAD_REQUEST; // 400
    return OK; // 200
}

int Server::isValidRequest(const HttpRequest &request) {
    int status;

    if ((status = isValidRequestLine(request)) != 200 ||
        (status = isValidRequestHeader(request)) != 200 ||
        (status = isValidRequestBody(request)) != 200)
        return status;

    return 200;
}