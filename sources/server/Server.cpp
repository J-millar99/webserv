#include "Server.hpp"

Server::~Server() { } /* Unused */
Server &Server::operator=(const Server &ref) {
    if (this != &ref) {
        listen_flag = ref.listen_flag;
        return_flag = ref.return_flag;
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
    listen_flag = ref.listen_flag;
    return_flag = ref.return_flag;
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
