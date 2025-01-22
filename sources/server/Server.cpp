#include "Server.hpp"

Server::~Server() { } /* Unused */
Server &Server::operator=(const Server &ref) {
    if (this != &ref) {
        port = ref.port;
        default_server = ref.default_server;
        server_names = ref.server_names;
        error_pages = ref.error_pages;
        limit_client_body_size = ref.limit_client_body_size;
        locations = ref.locations;

        server_socket = ref.server_socket;
        server_addr = ref.server_addr;
        memcpy(buffer, ref.buffer, BUFFER_SIZE);
    }
    return *this;
}

Server::Server(const Server &ref) {
    port = ref.port;
    default_server = ref.default_server;
    server_names = ref.server_names;
    error_pages = ref.error_pages;
    limit_client_body_size = ref.limit_client_body_size;
    locations = ref.locations;
    
    server_socket = ref.server_socket;
    server_addr = ref.server_addr;
    memcpy(buffer, ref.buffer, BUFFER_SIZE);
}

Server::Server() {
    port = 0;
    limit_client_body_size = -1;
    default_server = false;
}

void Server::settingServer() {
    // 논블로킹 소켓 서버 설정
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("socket error");
    // SO_REUSEADDR 옵션 설정
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt error");
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
    HttpRequest request = recvHttpRequest(client_socket);
    HttpResponse response = createHttpResponse(request);
    request.printInfo();
    
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