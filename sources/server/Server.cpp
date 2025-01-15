#include "Server.hpp"

Server &Server::operator=(const Server &ref) { (void)ref; return *this; } /* Unused */
Server::~Server() { /* Unused */ }
Server::Server(const Server &ref) {
    this->port = ref.port;
    this->server_names = ref.server_names;
    this->error_pages = ref.error_pages;
    this->limit_client_body_size = ref.limit_client_body_size;
    this->locations = ref.locations;
}
Server::Server() {
    port = 0;
    limit_client_body_size = -1;
}

void Server::settingServer() {
    // 논블로킹 소켓 서버 설정
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("socket error");
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl F_SETFL error");

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // 소켓에 주소 할당
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        throw std::runtime_error("bind error");
    
    // 리스닝 시작
    if (listen(server_socket, MAX_CLIENTS) == -1)
        throw std::runtime_error("listen error");
}

void Server::handleClient(int client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t n = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (n <= 0) {
        close(client_socket);
        return;
    }

    // HTTP 응답 헤더 생성
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html; charset=UTF-8\r\n";
    response += "Connection: close\r\n";
    
    // 응답 본문 생성
    std::string body = "<html>\r\n";
    body += "<head><title>WebServ Test</title></head>\r\n";
    body += "<body>\r\n";
    body += "<h1>Welcome to WebServ!</h1>\r\n";
    body += "<p>Server is running on port " + std::to_string(port) + "</p>\r\n";
    body += "</body>\r\n";
    body += "</html>\r\n";

    // Content-Length 헤더 추가
    response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    response += "\r\n"; // 헤더와 본문 구분을 위한 빈 줄
    response += body;

    // 응답 전송
    ssize_t total_sent = 0;
    ssize_t length = response.length();
    
    while (total_sent < length) {
        ssize_t sent = write(client_socket, response.c_str() + total_sent, length - total_sent);
        if (sent < 0) {
            close(client_socket);
            return;
        }
        total_sent += sent;
    }

    close(client_socket);
}