#include "Server.hpp"

Server &Server::operator=(const Server &ref) { /* Unused */
    (void)ref;
    return *this;
}
Server::~Server() { /* Unused */ }

Server::Server(const Server &ref) {
    this->port = ref.port;
    this->server_names = ref.server_names;
    this->error_page = ref.error_page;
    this->limit_client_body_size = ref.limit_client_body_size;
    this->locations = ref.locations;
}

Server::Server() {
    printInfo();
}
