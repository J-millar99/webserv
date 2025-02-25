#include "Server.hpp"

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


HttpResponse Server::createHttpResponse(const HttpRequest &request) {
    HttpResponse response;
    // 상태 코드 설정
    int status_code = isValidRequest(request);
    // 버전 설정
    if (status_code != OK)
        response.createErrorPage(status_code);
    else
        response.createPage(request);

    return response;
}


void Server::handleClient(int client_socket) {
    HttpRequest request = recvHttpRequest(client_socket);
    HttpResponse response = createHttpResponse(request);
    
    // 응답 전송
    ssize_t total_sent = 0;
    ssize_t length = response.length();
    
    while (total_sent < length) {
        ssize_t sent = send(client_socket, response.message().c_str() + total_sent, length - total_sent, 0);
        if (sent < 0) {
            close(client_socket);
            // 서버에 문제가 생겼을 때 500번대 페이지 만들어 전송
            return ;
        }
        total_sent += sent;
    }
    close(client_socket);
}