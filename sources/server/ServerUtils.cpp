#include "Server.hpp"

void Server::printInfo() {
    std::cout << port << " ";
}

void Server::checkField() {
    if (port < 1)
        throw std::runtime_error("port is not allocated");
    else if (limit_client_body_size == -1)
        throw std::runtime_error("limit_client_body_size is not allocated");
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        if (it->url_type == "" || it->root == "" || it->index == "" || it->methods == 0)
            throw std::runtime_error("all location directives must have be defined");
    }
}

int Server::getPort() const {
    return port;
}

int Server::getServerSocket() const {
    return this->server_socket;
}

bool Server::isServerSocket(int socket_fd) const {
    return socket_fd == server_socket; 
}

bool Server::getDefaultServer() const{
    return default_server;
}

void Server::setDefaultServer() {
    default_server = true;
}