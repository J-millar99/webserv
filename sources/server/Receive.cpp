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
    
    size_t total_bytes = 0;
    std::string raw_request;
    while (true) {
        ssize_t n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            close(client_socket);
            return request;
        }
        
        raw_request.append(buffer, n);
        
        // 헤더의 끝(\r\n\r\n)을 찾습니다
        if (raw_request.find("\r\n\r\n") != std::string::npos)
            break;
            
        total_bytes += n;
        if (total_bytes > MAX_HEADER_SIZE) {  // 헤더 크기 제한
            close(client_socket);
            return request;
        }
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
    
    if (!request.parseHttpRequest()) {
        close(client_socket);
    }
    return request;
}

// 에러 코드는 구체적 명시, 성공 코드는 200으로 설정
int Server::isValidRequestLine(const HttpRequest &request) {
    // 메소드 검증
    std::string request_method = request.getMethod();
    if (request_method == "GET" || request_method == "POST" || request_method == "DELETE")
        return 405; // Method Not Allowed

    // URI 검증
    std::string request_uri = request.getUri();
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        // uri가 일치하면 메소드 검증
        if (request_uri == it->url_type)
            if (it->methods & methodBitConverter(request.getMethod()) != 0)
                return 200; // OK
            else
                return 405; // Method Not Allowed
    }
    return 404; // Not Found
}

// 바디 사이즈가 Content-Length보다 크면 413 반환
int Server::isValidRequestBody(const HttpRequest &request) {
    // Content-Length 헤더가 있는 경우
    HttpHeader headers = request.getHeaders();
    if (headers.getHeader("Content-Length") != "")
        if (request.getBody().size() > limit_client_body_size)
            return 413; // Request Entity Too Large
    return 200; // OK
}

int Server::isValidRequest(const HttpRequest &request) {

    // 요청 메소드와 URI 검증
    isValidRequestLine(request);
    // 헤더 검증

    // 바디 검증
    isValidRequestBody(request);
    return 200; // OK
}