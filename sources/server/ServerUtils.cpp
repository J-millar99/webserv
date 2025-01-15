#include "Server.hpp"

void Server::printInfo() {
    std::cout << "Port: " << port << std::endl;
    std::cout << "Server names: ";
    for (std::list<std::string>::iterator it = server_names.begin(); it != server_names.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    for (std::list<std::string>::iterator it = error_pages.begin(); it != error_pages.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "Limit client body size: " << limit_client_body_size << std::endl;
    std::cout << "Locations: " << std::endl;
    for (std::list<Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
        std::cout << "Url_type: " << it->url_type << std::endl;
        std::cout << "Root: " << it->root << std::endl;
        std::cout << "Index: " << it->index << std::endl;
        std::cout << "Methods: ";
        if (it->methods & GET)
            std::cout << "GET ";
        if (it->methods & POST)
            std::cout << "POST ";
        if (it->methods & DELETE)
            std::cout << "DELETE ";
        std::cout << std::endl;
        std::cout << "Autoindex: " << (it->auto_index ? "on" : "off") << std::endl;
    }
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

int Server::getServerSocket() const {
    return this->server_socket;
}

bool Server::isServerSocket(int socket_fd) const {
    return socket_fd == server_socket; 
}