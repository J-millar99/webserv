#include "System.hpp"

System::System() { } /* Unused */ 
System::System(const System &ref) {(void)ref;} /* Unused */
System &System::operator=(const System &ref) { (void)ref; return *this; } /* Unused */
System::~System() { /* Unused */ }

System::System(int argc, char *argv[]) {
    if (!argv[1])   // 인자가 없을 경우 디폴트 config파일
        argv[1] = (char *)DEFAULT_CONFIG_FILE;
    checkArgumentNumber(argc);  // 인자 개수 확인
    checkConfigFileValidate(argv[1]);   // 파일 권한과 여부 확인
    parseConfigFile(argv[1]);   // 파싱
    // for (std::list<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
    //     it->printInfo();
    runServers();
}

void System::runServers() {
    if ((kq = kqueue()) == -1)
        throw std::runtime_error("kqueue error");
    
    for (std::list<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        it->settingServer();

        // 각 서버의 소켓을 kqueue에 등록
        struct kevent server_event;
        EV_SET(&server_event, it->getServerSocket(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        if (kevent(kq, &server_event, 1, NULL, 0, NULL) == -1)
            throw std::runtime_error("kevent registration failed"); 

        // 소켓과 서버를 매핑
        socket_to_server[it->getServerSocket()] = &(*it);
    }

    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    while (1) {
        struct kevent event_list[MAX_EVENTS];
        int new_events = kevent(kq, NULL, 0, event_list, MAX_EVENTS, &timeout);
        
        if (new_events == -1) {
            if (errno == EINTR) continue;  // Interrupted system call
            throw std::runtime_error("System kevent error");
        }

        for (int i = 0; i < new_events; i++) {
            int current_socket = event_list[i].ident;
            
            // Check if this is a server socket
            if (socket_to_server.find(current_socket) != socket_to_server.end() &&
                socket_to_server[current_socket]->isServerSocket(current_socket)) {
                
                Server* current_server = socket_to_server[current_socket];
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                int client_socket = accept(current_socket, (struct sockaddr*)&client_addr, &client_len);
                
                if (client_socket == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Accept error: " << strerror(errno) << std::endl;
                    }
                    continue;
                }

                // Set non-blocking mode for client socket
                int flags = fcntl(client_socket, F_GETFL, 0);
                if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
                    close(client_socket);
                    continue;
                }

                // Register client socket for reading
                struct kevent client_event;
                EV_SET(&client_event, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                
                if (kevent(kq, &client_event, 1, NULL, 0, NULL) == -1) {
                    close(client_socket);
                    continue;
                }

                socket_to_server[client_socket] = current_server;
            }
            else {
                // Handle client socket event
                if (event_list[i].flags & (EV_ERROR | EV_EOF)) {
                    close(current_socket);
                    socket_to_server.erase(current_socket);
                    continue;
                }

                Server* current_server = socket_to_server[current_socket];
                if (current_server) {
                    current_server->handleClient(current_socket);
                }
                socket_to_server.erase(current_socket);
            }
        }
    }
}